from Bus import Bus


# MSI监听协议
class Cache:
    block_num = 2
    # block_size = 2
    block = {
        "status": "I",
        "mem_addr": None,
        "data": None
    }

    def __init__(self):
        self.blocks = list()
        for i in range(0, self.block_num):
            self.blocks.append(dict(self.block))
        self.bus = None
        self.pid = None

    def set_pid(self, pid):
        self.pid = pid

    def set_bus(self, bus):
        self.bus = bus

    def read(self, mem_addr):
        for block in self.blocks:
            if block["status"] != "I" and block["mem_addr"] == mem_addr:
                print("read hit for address " + str(mem_addr))
                return block["data"]
        return self.read_miss(mem_addr)

    def write(self, mem_addr, data):
        for block in self.blocks:
            if block["status"] == "M" and block["mem_addr"] == mem_addr:
                print("write hit for address " + str(mem_addr))
                block["data"] = data
                return
            if block["status"] == "S" and block["mem_addr"] == mem_addr:
                print("write hit for address " + str(mem_addr))
                self.invalidate(mem_addr)
                block["data"] = data
                block["status"] = "M"
                return
        self.write_miss(mem_addr, data)

    def read_miss(self, mem_addr):
        print("read miss for address " + str(mem_addr))
        data = self.bus.read_miss(mem_addr, self.pid)
        for i in range(len(self.blocks)):
            if self.blocks[i]["status"] == "I":
                new_block = dict(self.block)
                new_block["status"] = "S"
                new_block["mem_addr"] = mem_addr
                new_block["data"] = data
                self.blocks[i] = new_block
                return data
        for i in range(len(self.blocks)):
            if self.blocks[i]["status"] == "S":
                self.evict(i)
                new_block = dict(self.block)
                new_block["status"] = "S"
                new_block["mem_addr"] = mem_addr
                new_block["data"] = data
                self.blocks[i] = new_block
                return data
        for i in range(len(self.blocks)):
            if self.blocks[i]["status"] == "M":
                self.evict(i)
                new_block = dict(self.block)
                new_block["status"] = "S"
                new_block["mem_addr"] = mem_addr
                new_block["data"] = data
                self.blocks[i] = new_block
                return data

    def write_miss(self, mem_addr, data):
        print("write miss for address " + str(mem_addr))
        self.bus.write_miss(mem_addr, data)
        for i in range(len(self.blocks)):
            if self.blocks[i]["status"] == "I":
                new_block = dict(self.block)
                new_block["status"] = "M"
                new_block["mem_addr"] = mem_addr
                new_block["data"] = data
                self.blocks[i] = new_block
                return data
        for i in range(len(self.blocks)):
            if self.blocks[i]["status"] == "S":
                self.evict(i)
                new_block = dict(self.block)
                new_block["status"] = "M"
                new_block["mem_addr"] = mem_addr
                new_block["data"] = data
                self.blocks[i] = new_block
                return data
        for i in range(len(self.blocks)):
            if self.blocks[i]["status"] == "M":
                self.evict(i)
                new_block = dict(self.block)
                new_block["status"] = "M"
                new_block["mem_addr"] = mem_addr
                new_block["data"] = data
                self.blocks[i] = new_block
                return data

    def evict(self, block_addr):
        i = block_addr
        print("evict for address " + str(self.blocks[i]["mem_addr"]) + ", with block_addr " + str(block_addr))
        if self.blocks[i]["status"] == "I":
            return
        if self.blocks[i]["status"] == "S":
            return
        if self.blocks[i]["status"] == "M":
            self.write_back(self.blocks[i]["mem_addr"], self.blocks[i]["data"])

    def invalidate(self, mem_addr):
        print("invalidate for address " + str(mem_addr))
        self.bus.invalidate(mem_addr, self.pid)
        pass

    def echo_read_miss(self, mem_addr):
        print("echo_read_miss for address " + str(mem_addr))
        for block in self.blocks:
            if block["mem_addr"] == mem_addr and block["status"] == "S":
                block["status"] = "S"
                return block["data"]
            elif block["mem_addr"] == mem_addr and block["status"] == "M":
                self.write_back(mem_addr, block["data"])
                block["status"] = "S"
                return block["data"]

    def echo_write_miss(self, mem_addr):
        print("echo_write_miss for address " + str(mem_addr))
        for block in self.blocks:
            if block["mem_addr"] == mem_addr and block["status"] == "S":
                block["status"] = "I"
                # return block["data"]
            elif block["mem_addr"] == mem_addr and block["status"] == "M":
                self.write_back(mem_addr, block["data"])
                block["status"] = "I"
                # return block["data"]

    # in this condition,if my status is "M",no cache will signal "invalidate".
    # because "S" is clean, so don't need write_back
    def echo_invalidate(self, mem_addr):
        print("echo_invalidate for address " + str(mem_addr))
        for block in self.blocks:
            if block["mem_addr"] == mem_addr and block["status"] == "S":
                block["status"] = "I"

    def write_back(self, mem_addr, data):
        print("write_back for address " + str(mem_addr))
        self.bus.write_back(mem_addr, data)

    def status_table(self):
        print("block_addr\tstatus\tmem_addr\tdata")
        i = 0
        for block in self.blocks:
            print(str(i) + "\t\t\t" + str(block["status"]) + "\t\t" + str(block["mem_addr"]) + "\t\t\t" + str(
                block["data"]))
            i += 1

    def get_status_rows(self):
        status_rows = list()
        for i in range(0, len(self.blocks)):
            row = [i, self.blocks[i]["status"], self.blocks[i]["mem_addr"], self.blocks[i]["data"]]
            status_rows.append(row)
        return status_rows
