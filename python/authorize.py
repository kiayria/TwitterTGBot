from flask import Flask
from flask import request
from flask import redirect
import requests
from pymongo import MongoClient

app = Flask(__name__)

client = MongoClient()
db = client.test
client = MongoClient('mongodb://admin:admin@localhost:27017')
db = client.twibotdb
collection = db.users
@app.route('/')
def get_verifier():
    oauth_token_tmp = request.args.get('oauth_token')
    oauth_verifier = request.args.get('oauth_verifier')
    res = requests.post(
        'https://api.twitter.com/oauth/access_token?oauth_token=' + oauth_token_tmp + '&oauth_verifier=' + oauth_verifier)
    res_split = res.text.split('&')
    oauth_token = res_split[0].split('=')[1]
    oauth_secret = res_split[1].split('=')[1]
    userid = res_split[2].split('=')[1]
    username = res_split[3].split('=')[1]
    doc = collection.find_one_and_update(
        {"oauth_token": oauth_token_tmp},
        {"$set":
             {"oauth_token": oauth_token, "oauth_token_secret": oauth_secret, "user_id": userid, "screen_name": username}
         }, upsert=True
    )
    return redirect('http://t.me/twittertgbot')



if __name__ == "__main__":
    app.run(debug=True)
