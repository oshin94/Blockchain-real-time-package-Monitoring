from flask import Flask
from flask import request
import os
from blockchain import Blockchain
import pickle

app = Flask(__name__)

@app.route("/data", methods = ["POST"])
def index():
    if request.method == "POST":
        print("received, clock starts in POST")
        #time.sleep(1)
        print("received POST", request.data)
        #data = json.loads(request.data.decode("utf-8").replace("\r\n", ";"))
        print(request.data.decode("utf-8").replace("\r\n", ";"))
        #print(data["location"])
        
        stream = str(request.data.decode("utf-8").replace("\r\n",";"))
        
        l_data = stream.split(",")
        d_id = l_data[0]
        snd_frm = l_data[2]
        
        
        if d_id+".pkl" in os.listdir("./pickle/"):
            pass
        else: #create genesis block
            create_genesis_block(d_id)
        
        with open("recv_data.txt", "a") as f:
            f.write(stream+"\n")
        
        if snd_frm == '0': #GPS data
            with open("./pickle/"+d_id+".pkl", "rb") as f:
                blockchain = pickle.load(f)
            blockchain.add_new_block(l_data[1], l_data[3], l_data[4]);
            with open("./pickle/"+d_id+".pkl", "wb") as f:
                pickle.dump(blockchain, f, pickle.HIGHEST_PROTOCOL)
            del blockchain
                
        elif snd_frm == '1':
            datas = stream.split(";"); del datas[-1]
            with open("./pickle/"+d_id+".pkl", "rb") as f:
                blockchain = pickle.load(f)
            for idx, data in enumerate(datas):
                if idx == 0:
                    continue
                else:
                    datal = data.split(",")
                    print(datal)
                    time = datal[0]
                    lat = datal[1]
                    lng = datal[2]
                    
                    blockchain.add_new_block(time, lat, lng);
            with open("./pickle/"+d_id+".pkl", "wb") as f:
                pickle.dump(blockchain, f, pickle.HIGHEST_PROTOCOL)
            del blockchain
                    
        return("sucess")

def create_genesis_block(d_id):
    Device = Blockchain()
    with open("./pickle/"+d_id+".pkl", "wb") as f:
        pickle.dump(Device, f, pickle.HIGHEST_PROTOCOL)
    print("Pickle object for Device id:", d_id, ", created")
    del Device

if __name__ == "__main__":
    app.run(debug=True)