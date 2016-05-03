#!/usr/bin/python3

from flask import Flask
from flask import request

# Create an instance of Flask
app = Flask(__name__);

# Present a nice screen when someone is lost
@app.route("/")
def default_route():
    return "Welcome to the Coffee Build Server, where you may read error reports!";

# For servers, get their logs and put them in the database
@app.route("/logger/data/<arch>/<int:sid>",methods=["POST","GET"])
def build_log_data(arch,sid):
    if request.method == "GET":
        return "Are you lost?";
    return "You are: plat=%s,server_id=%s,payload=%s" % (arch,sid,request.data);

if __name__ == "__main__":
    app.run();
