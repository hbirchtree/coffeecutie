#!/usr/bin/python3

import os
import sqlite3

from flask import Flask
from flask import request
from flask import send_from_directory
from flask import url_for
from flask import g

# Create an instance of Flask
app = Flask(__name__);
app.config.from_object(__name__)

# Load default config and override config from an environment variable
app.config.update(dict(
    DATABASE=os.path.join(app.root_path, 'db/error_reports.db'),
    SECRET_KEY='development key',
    USERNAME='admin',
    PASSWORD='default'
));
app.config.from_envvar('FLASKR_SETTINGS', silent=True);

def connect_to_database():
    return sqlite3.connect(app.config['DATABASE']);

def open_db():
    db = getattr(g,'_database',None);
    if db == None:
        db = g._database = connect_to_database();
    db.row_factory = sqlite3.Row;
    return db;

def init_db():
    with app.app_context():
        db = open_db();
        with app.open_resource('db/schema.sql',mode='r') as f:
            db.cursor().executescript(f.read());
        db.commit();

def query_db(query,args=(),one=False):
    cur = open_db().execute(query,args);
    rd = cur.fetchall();
    cur.close();
    return (rv[0] if rv else None) if one else rv;

    def enter_report(obj):
        return;

@app.teardown_appcontext
def close_db(exception):
    db = getattr(g,'_database',None);
    if db != None:
        db.close();

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
