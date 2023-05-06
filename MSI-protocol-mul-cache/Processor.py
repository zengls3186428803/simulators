class Processor:
    def __init__(self):
        self.cache = None
        self.pid = None

    def set_pid(self, pid):
        self.pid = pid

    def set_cache(self, cache):
        self.cache = cache

    def read(self, mem_addr):
        return self.cache.read(mem_addr)

    def write(self, addr, data):
        return self.cache.write(addr, data)