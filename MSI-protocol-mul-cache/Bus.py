class Bus:
    def __init__(self):
        self.caches = None
        self.memory = None
        pass

    def set_caches(self, caches):
        self.caches = caches

    def set_memory(self, memory):
        self.memory = memory

    def read_miss(self, mem_addr, pid):
        for cache in self.caches:
            if cache.pid != pid:
                data = cache.echo_read_miss(mem_addr)
                if data is not None:
                    return data
        return self.memory.read(mem_addr)

    def write_miss(self, mem_addr, pid):
        for cache in self.caches:
            if cache.pid != pid:
                cache.echo_write_miss(mem_addr)

    def invalidate(self, mem_addr, pid):
        for cache in self.caches:
            if cache.pid != pid:
                cache.echo_invalidate(mem_addr)

    def write_back(self, mem_addr, data):
        self.memory.write(mem_addr, data)
