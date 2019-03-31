import python.make_components.command as cmd

class StatementBlock:
    def __init__(self):
        self.conditional = "eq (1, 1)"
        self.statements = []

    def serialize(self, indent = 0):
        block = '\n' + '\t' * indent + "if" + self.conditional + "\n"

        for statement in self.statements:
            if type(statement) == cmd.Command:
                pass

            block += '\t' * indent + statement.serialize(indent=indent + 1) + "\n"

        block += '\t' * indent + "endif\n"

        return block

if __name__ == "__main__":
    block = StatementBlock()

    cmd1 = cmd.Command()
    cmd1.command = "echo abc"

    block.conditional = "eq (2, 3)"
    block.statements = [cmd1]

    print(block.serialize())
