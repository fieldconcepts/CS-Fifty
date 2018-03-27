from flask import Flask, flash, redirect, render_template, request, session, url_for
import requests
from bs4 import BeautifulSoup
import math
from cs50 import SQL

from datetime import datetime, timedelta
import time
import datetime


from mpl_toolkits.basemap import Basemap
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import netCDF4



from helpers import *




#need to run the following before starting server: export API_KEY=AIzaSyCTv_5kytpdJ-oqwT6l8eAHdlx-GopcqFU
#slideshow from https://www.w3schools.com/howto/howto_js_slideshow.asp

app = Flask(__name__)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///buoys.db")

#global variables

LAST_SCRAPE = datetime.now()
DELTA = timedelta(minutes=15)

print ("LAST SCRAPE = {}".format(LAST_SCRAPE))
print ("DELTA = {}".format(DELTA))


#do iniital web scrape when starting server and cache all data in csv files. comment out when developing to avoid too many http requests
grab_latest_data()


@app.route("/")
def index():

    return render_template("index.html")

@app.route("/data", methods=["GET", "POST"])
def data():

    global LAST_SCRAPE, DELTA

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        #get the symbol fromm user input
        buoy = request.form.get("buoy")



        #get lat/lon/name from database
        rows = db.execute("SELECT * FROM buoys WHERE id = :id", id=buoy)
        lat = rows[0]["lat"]
        lon = rows[0]["lon"]
        name = rows[0]["name"]



        #get the current time
        NOW = datetime.now()

        #check if the time of the last scrape is less than the delta
        if NOW - LAST_SCRAPE < DELTA:

            #if true then pull data from the cached data

            print ("Time since last scrape = {}".format(NOW - LAST_SCRAPE))
            print ("Getting data from cache")

            FILE = "{}.csv".format(buoy)

            twentyfourdata = pd.read_csv(FILE)
            print("Reading in {}.csv...".format(buoy))

            #grab time and wave height data and save as a list
            labels = twentyfourdata['Time (GMT)'].values.tolist()
            values = twentyfourdata['Wave Height (m)'].values.tolist()
            v_max = twentyfourdata['Max Wave Height (m)'].values.tolist()

            #reverse lists order
            labels.reverse()
            values.reverse()
            v_max.reverse()

            #feed place name and the data array tables into the render method
            return render_template("data-response.html", place=name, lat=lat, lon=lon, values=values, labels=labels, v_max = v_max, tables=[twentyfourdata.to_html(index=False, classes='table-striped table')])


        #if false then get a fresh dataset
        print ("Pulling fresh data from web")

        LAST_SCRAPE = NOW

        print ("Last scrape time updated")

        # call lookup functions for latest data and twnety four hours dataframes pandas object

        twentyfourdata = getdata(buoy, 2)

        with open('107.csv', 'w') as f:
            twentyfourdata.to_csv(f, index=False)
        print("Updated cached CSV file")


        labels = twentyfourdata['Time (GMT)'].values.tolist()
        values = twentyfourdata['Wave Height (m)'].values.tolist()
        v_max = twentyfourdata['Max Wave Height (m)'].values.tolist()


        labels.reverse()
        values.reverse()
        v_max.reverse()




        #feed place name and the data array tables into the render method
        return render_template("data-response.html", place=name, lat=lat, lon=lon, values=values, labels=labels, v_max = v_max, tables=[twentyfourdata.to_html(index=False, classes='table-striped table')])

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("data.html")


@app.route("/charts", methods=["GET", "POST"])
def charts():

    #for i in range(8):
    #    create_chart(i)



    today = datetime.now()
    t= today.strftime('%Y-%m-%d')



    return render_template("charts.html", t=t)

@app.route("/about")
def about():

    return render_template("about.html")

if __name__ == "__main__":
    app.run()