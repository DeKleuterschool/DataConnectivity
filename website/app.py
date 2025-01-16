from flask import json
from flask import Flask, render_template, request,redirect
import datetime
import database as db
app = Flask(__name__)

@app.route("/")
def index():
     bikelat = 53.2194
     bikelng = 6.5665
     bikedate = datetime.datetime(2025, 1, 6, 14, 20)
     bikeyear = bikedate.year
     bikemonth = bikedate.month
     bikeday = bikedate.day
     bikehour = bikedate.hour
     bikeminute = bikedate.minute
     return render_template("index.html", bikelat=bikelat, bikelng=bikelng, bikeyear=bikeyear, bikemonth=bikemonth, bikeday=bikeday, bikehour=bikehour, bikeminute=bikeminute)

@app.route("/", methods=["POST"])
def index_post():
     bikedate_str = request.form["bikedate"]
     bikedate = datetime.datetime.strptime(bikedate_str, "%Y-%m-%dT%H:%M")
     print(bikedate)
     bike = db.get_bike(bikedate)
     
     if bike:
         bike_date = datetime.datetime.strptime(bike["date"], "%Y-%m-%d %H:%M:%S")
         bikelat = bike["lat"]
         bikelng = bike["lng"]
         bikeyear = bike_date.year
         bikemonth = bike_date.month
         bikeday = bike_date.day
         bikehour = bike_date.hour
         bikeminute = bike_date.minute
         return render_template("index.html", bikelat=bikelat, bikelng=bikelng, bikeyear=bikeyear, bikemonth=bikemonth, bikeday=bikeday, bikehour=bikehour, bikeminute=bikeminute)
     return redirect("/")
     
@app.route("/receivedata", methods=["POST"])     
def receiveDataPost():
     data = request.get_json()
     db.new_bike(data["lat"], data["lng"], datetime.datetime.now())
     resdata = dict()
     resdata["success"] = "success"
     response = app.response_class(
        response=json.dumps(resdata),
        status=200,
        mimetype='application/json'
     )
     return response

if __name__ == "__main__":
     app.run(host="0.0.0.0", port=80, debug=True)