class Memory:
    def __init__(self):
        self.M = dict()

    def read(self, mem_addr):
        return self.M.get(mem_addr, 0)

    def write(self, mem_addr, data):
        self.M[mem_addr] = data
