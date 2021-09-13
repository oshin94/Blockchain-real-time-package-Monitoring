from flask import Flask, render_template, url_for
import pickle
from pathlib import Path
import json

app = Flask(__name__)

@app.route("/")
def index():
    return render_template("index.html",)

@app.route("/device1")
def device1():
    data = []
    my_file = Path("../pickle/a1b2c.pkl")
    if my_file.is_file():
        with open("../pickle/a1b2c.pkl", "rb") as f:
            bc = pickle.load(f)
        for block in bc.get_blocks():
            #print("data:", block.to_string().split())
            data.append([float(block.to_string().split()[1]), float(block.to_string().split()[2])])
        del data[0]
        print(data)
        #data = {"data": data}
        print(type(data))
        #dat = [[1, 1],[2,2]]
    return render_template("device1.html", data=json.dumps(data))
    
@app.route("/device2")
def device2():
    return render_template("device2.html")

if __name__ == "__main__":
    app.run(host="0.0.0.0", debug=True, port=5001)