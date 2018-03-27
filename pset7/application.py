from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, url_for
from flask_session import Session
from passlib.apps import custom_app_context as pwd_context
from tempfile import mkdtemp
import time

from helpers import *

# configure application
app = Flask(__name__)

# ensure responses aren't cached
if app.config["DEBUG"]:
    @app.after_request
    def after_request(response):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Expires"] = 0
        response.headers["Pragma"] = "no-cache"
        return response

# custom filter
app.jinja_env.filters["usd"] = usd

# configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")



# ----------------------INDEX

@app.route("/")
@login_required
def index():

    #get a list of the the shares and the sum value of the quanitites and group them by symbol for the unique user id
    rows = db.execute("SELECT SUM(quantity) as shares, symbol FROM portfolio WHERE user_id = :user_id GROUP BY symbol", user_id=session["user_id"])

    #get the curret cash value for the unique user id
    rows2 = db.execute("SELECT cash FROM users WHERE id = :id", id=session["user_id"])
    funds = rows2[0]["cash"]

    #empty list that we will feed a list of dictionaries into
    stocks = []

    #counter for total price of stocks (price*qty)
    grandtotal = 0

    #iterate through each row of the databse and look up the current price.
    for row in rows:
        quote = lookup(row["symbol"])
        qty = row["shares"]
        total = quote["price"] * qty
        grandtotal += total

        #if user has bought and sold shares the symbol still eists in db but quanity will equal 0. skip these lines.
        if qty == 0:
            continue

        #create a dictionary for each row in the db and append it to the stocks list.
        else:
            stock_dict = {"symbol": row["symbol"], "name": quote["name"], "quantity": qty, "price": usd(quote["price"]), "total": usd(total)}
            stocks.append(stock_dict)

    #add the total value of the current sotck prices and the cash
    grandtotal2 = grandtotal + funds

    #render the inde template feeding in the stocks[] list, cash and grandtotal2. jinja will loop  through the sotcks list and print html table rows.
    return render_template("index.html", stocks=stocks, cash=usd(funds), grandtotal=usd(grandtotal2))



# ----------------------BUY

@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        #get symbol and quantity from user, lookup current stock price
        getquote = request.form.get("symbol")
        quotedict = lookup(getquote)
        quantity = request.form.get("qty")

        #check if symbol exists
        if quotedict == None:
            return apology("Symbol not found!")

        #check if quantity to buy is a postive number
        if not quantity.isnumeric():
            return apology("Please enter positve number")

        #get the values from the dictionary returned from the lookup() fucntion
        qty = int(quantity)
        nam = list(quotedict.values())[0]
        pri = list(quotedict.values())[1]
        sym = list(quotedict.values())[2]
        cost = qty * pri

        #use the time module to get the crrent date time.
        thetime = time.strftime('%Y-%m-%d %H:%M:%S')

        #lookup the cash value for the unique user id
        rows = db.execute("SELECT cash FROM users WHERE id = :id", id=session["user_id"])
        funds = rows[0]["cash"]

        #check that there is enough cash to buy the stock
        if funds < cost:
            return apology("Not enough cash")

        #if there is enough cash then add a new row to the portfolio database including symbol, qty, price time, name
        db.execute("INSERT INTO portfolio (user_id, symbol, quantity, price, datetime, name) VALUES(:user_id, :symbol, :quantity, :price, :datetime, :name)", user_id=session["user_id"], symbol=sym, quantity=qty, price=pri, datetime=thetime, name=nam)

        #subttract the cost of the new purchase from the cash value
        remainder = funds - cost

        #update cash value for in the user db
        db.execute("UPDATE users SET cash = :remainder WHERE id = :id", remainder=remainder, id=session["user_id"])

        #redirect to index page when done
        return redirect(url_for("index"))


    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("buy.html")

    #if something goes wrong print error message
    return apology("Couldn't BUY")



# ----------------------HISTORY

@app.route("/history")
@login_required
def history():

    #load in the symbol,quantity, price and datetime fields from the portfolio for the unique id
    rows = db.execute("SELECT symbol, quantity, price, datetime FROM portfolio WHERE user_id = :user_id", user_id=session["user_id"])

    #create an emmpty stocks[] list
    stocks = []

    #iterate throught the rows returned and create a dictionary fro each row, appedning to the stocks[] list
    for row in rows:

        STOCK_DICT = {"symbol": row["symbol"],  "quantity": row["quantity"], "price": row["price"], "time": row["datetime"]}
        stocks.append(STOCK_DICT)

    #feed the stocks list into the history html page usning jinja to loop throught the stocks[] list and printing table rows
    return render_template("history.html", stocks=stocks)



# ----------------------LOGIN

@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in."""

    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password")

        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))

        # ensure username exists and password is correct
        if len(rows) != 1 or not pwd_context.verify(request.form.get("password"), rows[0]["hash"]):
            return apology("invalid username and/or password")

        # remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")



# ----------------------LOGOUT

@app.route("/logout")
def logout():
    """Log user out."""

    # forget any user_id
    session.clear()

    # redirect user to login form
    return redirect(url_for("login"))



# ----------------------QUOTE

@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

     # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        #get the symbol fromm user input
        getquote = request.form.get("quote")

        #use lookup() function to return a dictionary of current price, name, ticker symbol
        quotedict = lookup(getquote)

        #check symbool is valid
        if quotedict == None:
            return apology("Symbol not found!")

        #get values from the returned dictionary
        nam = list(quotedict.values())[0]
        pri = list(quotedict.values())[1]
        sym = list(quotedict.values())[2]

        #feed name, price and symbol into the quote,htmt template
        return render_template("quoted.html", name=nam, price=pri, symbol=sym)

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("quote.html")



# ----------------------REGISTER

@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user."""

    # forget any user_id
    session.clear()

    #if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("Missing username!")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("Missing password!")

        #check that both the passwords actually match
        elif request.form.get("password") != request.form.get("passwordconf"):
            return apology("Passwords do not match!")

        #encrypt the password
        hash = pwd_context.hash(request.form.get("password"))

        #query database for to find if username already exists
        result = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
        if result:
            return apology("Username already taken!")

        #insert new user and hash into the database
        db.execute("INSERT INTO users (username, hash) VALUES(:username, :hash)", username=request.form.get("username"), hash=hash)

        #read back the user in the database by username and save their "id" in coloumn 0 at the session "user_id"
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
        session["user_id"] = rows[0]["id"]


        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")



# ----------------------SELL

@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():

    #if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        #get the symbol and quantity from user form
        getquote = request.form.get("symbol")
        quantity = request.form.get("qty")

        #use lookup fucntion to get ticker,name,curernt price from symbol, returns info as a dictionary of 3 keys.
        quotedict = lookup(getquote)

        #check symbol is valid
        if quotedict == None:
            return apology("Symbol not found!")

        #check qunatity is a number and positive
        if not quantity.isnumeric():
            return apology("Please enter positve number")

        #get values from the retured dictionary
        qty = int(quantity)
        nam = list(quotedict.values())[0]
        pri = list(quotedict.values())[1]
        sym = list(quotedict.values())[2]
        cost = qty * pri
        thetime = time.strftime('%Y-%m-%d %H:%M:%S')

        #check stock is actually held by user in portfolio db, sum the quantitiy of all the shares of that symbol belonging to the user id
        result = db.execute("SELECT SUM(quantity) as shares FROM portfolio WHERE user_id = :user_id AND symbol = :symbol", user_id=session["user_id"], symbol=sym)
        if not result[0]["shares"]:
            return apology("You do not own that stock")

        #check that you have enough shares in portfolio db to sell
        if qty > result[0]["shares"]:
            return apology("You do not own enough stock")

        #insert data from trade into the portfolio database making ure the quantity is negative
        db.execute("INSERT INTO portfolio (user_id, symbol, quantity, price, datetime, name) VALUES(:user_id, :symbol, :quantity, :price, :datetime, :name)", user_id=session["user_id"], symbol=sym, quantity=-qty, price=pri, datetime=thetime, name=nam)

        #get cash vaue from user database by using the session id
        rows = db.execute("SELECT cash FROM users WHERE id = :id", id=session["user_id"])

        #get cash value
        funds = rows[0]["cash"]

        #add the cost of selling the shares to the funds
        remainder = funds + cost

        #update the user database with new cash value
        db.execute("UPDATE users SET cash = :remainder WHERE id = :id", remainder=remainder, id=session["user_id"])

        #finish by redirecting to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("sell.html")

    #if something goes wrong print error message
    return apology("Couldn't SELL")



# ----------------------DEPOSIT

@app.route("/deposit", methods=["GET", "POST"])
@login_required
def deposit():

    #if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        #get user input for amount to deposit
        quantity = request.form.get("qty")

        #check user input is a positive number
        if not quantity.isnumeric():
            return apology("Please enter positve number")

        #convert string to int
        depos = int(quantity)

        #get current cash value for user
        rows = db.execute("SELECT cash FROM users WHERE id = :id", id=session["user_id"])
        funds = rows[0]["cash"]

        #add the deposit value to cash
        remainder = funds + depos

        #update user db with new cash total
        db.execute("UPDATE users SET cash = :remainder WHERE id = :id", remainder=remainder, id=session["user_id"])

        #redirect to homepage
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("deposit.html")

    #print error message if something goes wrong
    return apology("Couldn't DEPOSIT")



# ----------------------WITHDRAW

@app.route("/withdraw", methods=["GET", "POST"])
@login_required
def withdraw():

    #if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        #get user input for amount to deposit
        quantity = request.form.get("qty")

        #check if input is a positive number
        if not quantity.isnumeric():
            return apology("Please enter positve number")

        #convert str to int
        wdraw = int(quantity)

        #get current cash value from user db
        rows = db.execute("SELECT cash FROM users WHERE id = :id", id=session["user_id"])
        funds = rows[0]["cash"]

        #check that the withdraw request is less than the total cash
        if funds < wdraw:
            return apology("Not enough funds")

        #update the cash by subtracting the withdrawn amount
        remainder = funds - wdraw

        #update user db with new cash total
        db.execute("UPDATE users SET cash = :remainder WHERE id = :id", remainder=remainder, id=session["user_id"])

        #redirect to homepage
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("withdraw.html")

    #error message if somethin goes wrong
    return apology("Couldn't WITHDRAW")