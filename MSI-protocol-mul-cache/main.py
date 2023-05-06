from Memory import Memory
from Cache import Cache
from Processor import Processor
from Bus import Bus
from show_status_table import *
from Interpreter import Interpreter

# parameters
n = 3  # the number of processors
Cache.block_num = 3  # the number of blocks of each cache

# assemble computer
processors = list()
caches = list()
memory = Memory()
bus = Bus()
bus.set_memory(memory)
for i in range(0, 3):
    cache = Cache()
    cache.set_pid(i)
    cache.set_bus(bus)
    caches.append(cache)

    processor = Processor()
    processor.set_pid(i)0
    processor.set_cache(cache)
    processors.append(processor)
bus.set_caches(caches)

# initial status stable of cache
show_status_tables(caches)

# give the computer an interpreter
interpreter = Interpreter()
interpreter.set_processors(processors)

# pre command(optional)
# pre_command = ["0 r 10", "0 w 20 200", "1 w 30 300"]
# for c in pre_command:
#     interpreter.interpret(c)
# show_status_tables(caches)

print("command format: <processor_id> <space> <operation(read/write/r/R/w/W)> <space> <mem_addr> [<space> <data>]")
print("enter 'quit' or 'exit' to end")
print("such as, '0 read 99','1 write 99 80', '1 r 77'")
while True:
    command = str(input())
    if command == "":
        continue
    if command == "exit" or command == "quit":
        break
    interpreter.interpret(command)
    show_status_tables(caches)
