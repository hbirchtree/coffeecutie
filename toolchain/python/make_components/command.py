class COMMAND_FLAGS:
    IGNORED  = 1
    SILENCED = 2

class Command:
    def __init__(self):
        self.command = "true"
        self.flags = 0

    def serialize(self):
        flagged = ""

        if self.flags & COMMAND_FLAGS.IGNORED:
            flagged = "-"

        if self.flags & COMMAND_FLAGS.SILENCED:
            flagged = "@"

        command = ""
        num_frags = 0

        for frag in self.command.split('\n'):
            command += frag + " \\\n\t"
            num_frags += 1

        if num_frags > 0:
            command = command[:-4]

        return '\t' + flagged + command.strip()

class Target:
    def __init__(self, title = 'NOTHING'):
        self.target_name = title
        self.dependencies = []
        self.commands = []

    def serialize(self):
        target_description = self.target_name + ": "

        for dep in self.dependencies:
            target_description += dep.target_name + " "

        target_description = target_description.strip() + "\n"

        for cmd in self.commands:
            target_description += cmd.serialize() + "\n"

        return target_description

if __name__ == "__main__":
    cmd = Command()

    cmd.command = "echo abc \\\n eh"
    cmd.flags = COMMAND_FLAGS.IGNORED

    t2 = Target()
    t2.target_name = "abc_456"

    t = Target()

    t.target_name = "abc_123"
    t.commands += [cmd]
    t.dependencies += [t2]

    print(t.serialize())
