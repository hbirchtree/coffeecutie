class VARFLAGS:
    ASSIGN   = 0
    DEFAULTS = 1

class VarDecl:
    def __init__(self):
        self.name = "NOTHING_NAME"
        self.var = "NOTHING_VAR"
        self.flag = VARFLAGS.ASSIGN

    def serialize(self, indent = 0):
        flag = ":"

        if self.flag & VARFLAGS.DEFAULTS:
            flag = "?"

        return "\t" * indent + self.name + " " + flag + "= " + self.var

if __name__ == "__main__":
    var = VarDecl()
    var.name = "BUILD_DIR"
    var.var = "abc/def"
    var.flag = VARFLAGS.DEFAULTS

    print(var.serialize())
    print(var.serialize(1))

    var.flag = VARFLAGS.ASSIGN
    print(var.serialize())