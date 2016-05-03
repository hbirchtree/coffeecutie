#!/usr/bin/python3

import os
import sqlite3

from flask import Flask
from flask import request
from flask import send_from_directory
from flask import url_for
from flask import g
from flask import jsonify
from flask import render_template

# Create an instance of Flask
app = Flask(__name__);
app.config.from_object(__name__);

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
    with open_db() as db:
        cur = db.execute(query,args);
        rd = cur.fetchall();
        cur.close();
        db.commit();
        return (rd[0] if rd else None) if one else rd;

def enter_report(obj):
    query_db("INSERT INTO BUILDREPORTS VALUES(NULL,?,?,?,?,?);",args=(obj['host'],obj['commit'],obj['platform'],obj['status'],obj['log']));
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
@app.route("/logger/data/<arch>",methods=["POST","GET"])
def build_log_data(arch):
    if request.method != "POST" or request.mimetype != 'application/json':
        return Response("Are you lost?",mimetype='text/plain');
    obj = request.get_json();
    logdata = ''
    try:
        # Truncate log if too long
        obj['log'] = obj['log'][len(obj['log'])-1000000:len(obj['log'])]
        obj['platform'] = arch;
        enter_report(obj);
    except KeyError:
        return jsonify({'status':1,'error': 'Malformed request!'});
    
    return jsonify({'status':0});

if __name__ == "__main__":
    app.run(debug=True);
