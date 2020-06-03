import turtle

tan = turtle.Turtle()

#function that creates the X.
def characterX():
    tan.setheading(45)
    tan.pendown()
    tan.forward(60)
    tan.setheading(225)
    tan.forward(120)
    tan.penup()
    tan.setheading(90)
    tan.penup()
    tan.forward(85)
    tan.setheading(315)
    tan.pendown()
    tan.forward(120)
#funtion that creates the O.
def characterO():
    tan.pendown()
    tan.circle(60)