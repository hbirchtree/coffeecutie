#!/usr/bin/python3

from flask import Flask

# Create an instance of Flask
app = Flask(__name__);

# Present a nice screen when someone is lost
@app.route("/")
def default_route():
    return "Welcome to the Coffee Build Server, where you may read error reports!";

# For servers, get their logs and put them in the database
@app.route("/logger/data/<string:arch>/<int:sid>")
def build_log_data(arch,sid):
    return "OMG! Someone messaged me!";

if __name__ == "__main__":
    app.run();
