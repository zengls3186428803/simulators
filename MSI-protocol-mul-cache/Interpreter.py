class Interpreter:
    def __init__(self):
        self.processors = None

    def set_processors(self, processors):
        self.processors = processors

    # command format: processor_id <space> read/write(r/R/W/w) <space> mem_addr [<space> data]
    def interpret(self, command_str):
        if command_str is None or command_str == "":
            return
        command_split = command_str.split(sep=" ")
        # print(command_split)
        pid = int(command_split[0])
        mem_addr = int(command_split[2])
        if command_split[1][0] == "r" or command_split[1][0] == "R":
            self.processors[pid].read(mem_addr)
        elif command_split[1][0] == "w" or command_split[1][0] == "w":
            data = command_split[-1]
            self.processors[pid].write(mem_addr, data)