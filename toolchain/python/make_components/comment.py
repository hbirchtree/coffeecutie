class Comment:
    def __init__(self, cmt = "Hello"):
        self.comment = cmt

    def serialize(self):
        out = ""

        for c in self.comment.split("\n"):
            out += "# " + c.strip() + "\n"

        return out[:-1]

if __name__ == "__main__":
    comment_block = Comment(
    """
    hello there
    this is a test
    """)

    print(comment_block.serialize())
