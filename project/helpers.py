import urllib.request
from flask import redirect, render_template, request, session, url_for
from functools import wraps
import requests
from bs4 import BeautifulSoup
import pandas as pd
from datetime import datetime, timedelta
import time

from mpl_toolkits.basemap import Basemap
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import netCDF4




def apology(message):
    """Renders message as an apology to user."""

    return render_template("apology.html", error=message)


def getdata(buoy, k):
    """Look up data from buoy id and table#."""

    try:
        url = "http://www.channelcoast.org/data_management/real_time_data/charts/?chart={}&tab=waves&disp_option=1".format(buoy)
        res = requests.get(url)
        soup = BeautifulSoup(res.content,'lxml')

        table = soup.find_all('table')[k] # Select the table# you're interested in
        df = pd.read_html(str(table))[0] #returns a single item list so need to select the element ie. [0]

        df1 = df.iloc[1:] #remove first line containting the headers
        df1.columns = ["Time (GMT)", "Latitude", "Longitude", "Wave Height (m)", "Max Wave Height (m)", "Tpeak (s)", "Tz (s)", "Peak Direction (degrees)", "Spread (degrees)", "Sea Temp (°C)"]

        df1 = df1.drop(['Latitude', 'Longitude'], axis=1) # remove lat and lon columns

        return df1

    except:
        return None



def today():

    thedate = datetime.now().strftime('%Y-%m-%d')

    return thedate

def deltaday(k):

    d = datetime.now() - timedelta(days=k)
    thedate = d.strftime('%Y-%m-%d')

    return thedate



def print_date_time():
    print (time.strftime("%A, %d. %B %Y %I:%M:%S %p"))


def grab_latest_data():

    buoy_ids = [98, 75, 76, 107, 113, 116]

    for i in range(len(buoy_ids)):
        print("Getting data for buoy {}".format(buoy_ids[i]))

        url = "http://www.channelcoast.org/data_management/real_time_data/charts/?chart={}&tab=waves&disp_option=1".format(buoy_ids[i])
        res = requests.get(url)
        soup = BeautifulSoup(res.content,'lxml')

        table = soup.find_all('table')[2] # Select the table# you're interested in

        df = pd.read_html(str(table))[0] #returns a single item list so need to select the element ie. [0]

        df = df.iloc[1:] #remove first line containting the headers
        df.columns = ["Time (GMT)", "Latitude", "Longitude", "Wave Height (m)", "Max Wave Height (m)", "Tpeak (s)", "Tz (s)", "Peak Direction (degrees)", "Spread (degrees)", "Sea Temp (°C)"]

        df = df.drop(['Latitude', 'Longitude'], axis=1) # remove lat and lon columns


        filename = "{}.csv".format(buoy_ids[i])

        with open(filename, 'w') as f:
            df.to_csv(f, index=False)

    print ("Data grab successful")



def create_chart(timestep):
    
    time_step = timestep
    
    plt.figure(figsize=(8,8))
	# set up the URL to access the data server.
	# See the NWW3 directory on NOMADS 
	# for the list of available model run dates.
	
    mydate = datetime.now().strftime("%Y%m%d")
    myhour = '00' #00, 06, 12, 18
	
    url='http://nomads.ncep.noaa.gov:9090/dods/wave/nww3/nww3'+mydate+'/nww3'+mydate+'_'+myhour+'z'
	
	# Extract the significant wave height of combined wind waves and swell
	# dirpwsfc, dirswsfc, htsgwsfc, perpwsfc, perswsfc, ugrdsfc, vgrdsfc, wdirsfc, windsfc, wvdirsfc, wvpersfc
	
    file = netCDF4.Dataset(url)
	
	
    lat  = file.variables['lat'][120:144]
    lon  = file.variables['lon'][264:288]
    wavedata = file.variables['htsgwsfc'][time_step,120:144,264:288]
    wavedir = file.variables['dirpwsfc'][time_step,120:144,264:288]
	
    fill_val = 0.5
	
	#interpolate the dead space by filling with a low fill_value
	
    for i  in range(len(lat)):
		#get top row of 2d array and print
        z = wavedata[i]
		
		#convert all -- values row to NaNs
        znan = np.ma.filled(z.astype(float), np.nan)
		
		#go through each element checking if == to NaN and convert to min va
        for j in range(len(lon)):
            if np.isnan(znan[j]):
                znan[j] = fill_val
		
		#update dataset
        wavedata[i] = znan

    
	#convert to m to feet
    wavedata = wavedata * 3.28084


    m=Basemap(projection='mill',lat_ts=10,llcrnrlon=lon.min(),urcrnrlon=lon.max(),llcrnrlat=lat.min(),urcrnrlat=lat.max(),resolution='i')
    x, y = m(*np.meshgrid(lon,lat))
	
	#work out the angle for the arrows
	
    u_com = 1-(np.sin(np.deg2rad(wavedir)))
    v_com = 1-(np.cos(np.deg2rad(wavedir)))
    uproj,vproj,xx,yy = m.transform_vector(u_com,v_com,lon,lat,24,24,returnxy=True,masked=True)
	
    m.drawcoastlines(linewidth=0.25)
    m.fillcontinents(color='0.8')
	
    levels=np.arange(0,50,2)
	
    cs = m.contourf(x, y, wavedata, levels, cmap=cm.bone_r)
    
    cbar = m.colorbar(cs, location='right',pad="5%")
	
    Q = m.quiver(xx,yy,uproj,vproj,scale=100,width=0.0015,pivot='tip', color='w')
	
    plt.axis('off')
	
    plt.savefig('static/charts/NE_Atl_T{}.png'.format(time_step), bbox_inches='tight', pad_inches=0)
    #plt.savefig('static/chart.png', bbox_inches='tight', pad_inches=0)
