import turtle
import random
screen = turtle.Screen()
tan = turtle.Turtle()
#makes menu border
#dimensions may be off in some IDEs (was made on repl.it)
tan.penup()
tan.goto(-250,150)
tan.setheading(0)
for x in range (1,3):
  tan.speed(10)
  tan.pendown()
  tan.forward(500)
  tan.right(90)
  tan.forward(300)
  tan.right(90)
#creates content within the menu
tan.penup()
tan.goto(-65,110)
tan.write('Menu', font = ('ariel', 40, 'normal'))
tan.goto(-65,100)
tan.pendown()
tan.forward(130)
tan.penup()
tan.goto(-65,65)
tan.write('Hangman', font = ('ariel', 25, 'normal'))
tan.goto(-75,15)
tan.write('Tic Tac Toe', font = ('ariel', 25, 'normal'))
gameSelect = input('Select the game you want to play!')
if gameSelect == 'Hangman' or gameSelect == 'hangman'or gameSelect == 'HANGMAN':
#Start of hangman game
  playAgain = 'yes'
  #loop that gives player option to continue playing or stop
  while playAgain == 'yes' or playAgain == 'Yes':
   #creates diagram for hangman
    tan.clear()
    tan.speed(5)
    tan.setheading(0)
    tan.penup()
    tan.goto(-175,-100)
    tan.pendown()
    tan.color('black')
    tan.goto(150,-100)
    tan.penup()
    tan.goto(-25,-100)
    tan.pendown()
    tan.goto(-25,200)
    tan.goto(75,200)
    tan.goto(75,125)
    #finding if game is singleplayer or multiplayer from user
    print('Ok. Let''s get started! Make sure not to repeat letters!')
    #variables that determine how many times the loop will run. Once out of lives or letter correct = the number of letters that the word contains, the loop will end.
    lives = 0
    letterCorrect = 0
    #list of the words used in the hangman game.
    list = ['technology', 'software', 'intel', 'progress', 'advances', 'program', 'design', 'language', 'future', 'transform']
    #selects a random number, each of which represents a word from the list.
    num = random.randrange(len(list))
    word = list[num]
    tan.penup()
    tan.goto(-100,-175)
    #creates the number of dashes given by the word.
    for x in word:
      tan.pendown()
      tan.forward(15)
      tan.penup()
      tan.forward(10)
    print('The word has been selected! Guess away!')
    #loop that forces game to continue until the user runs out of tries and the full person is drawn, or until user guesses the word correctly.
    while lives <= 6 and letterCorrect != len(word):
      guess = input('Enter a Lower-case letter.')
      #these if statements scan to see whether the user guessed a letter that is correct or not.
      if (guess == word[0]):
        letterCorrect = letterCorrect + 1
        print('Correct!')
        tan.penup()
        tan.goto(-97,-170)
        tan.pendown()
        tan.write(word[0], font = ('ariel', 16, 'normal'))
        tan.penup()
        tan.hideturtle()
      if guess == word[1]:
        letterCorrect = letterCorrect + 1
        print('Correct!')
        tan.penup()
        tan.goto(-72,-170)
        tan.pendown()
        tan.write(word[1], font = ('ariel', 16, 'normal'))
        tan.penup()
        tan.hideturtle()
      if guess == word[2]:
        letterCorrect = letterCorrect + 1
        print('Correct!')
        tan.penup()
        tan.goto(-47,-170)
        tan.pendown()
        tan.write(word[2], font = ('ariel', 16, 'normal'))
        tan.penup()
        tan.hideturtle()
      if guess == word[3]:
        letterCorrect = letterCorrect + 1
        print('Correct!')
        tan.penup()
        tan.goto(-22,-170)
        tan.pendown()
        tan.write(word[3], font = ('ariel', 16, 'normal'))
        tan.hideturtle()
      if guess == word[4]:
        letterCorrect = letterCorrect + 1
        print('Correct!')
        tan.penup()
        tan.goto(3,-170)
        tan.pendown()
        tan.write(word[4], font = ('ariel', 16, 'normal'))
        tan.hideturtle()
      if len(word) > 5:
        if guess == word[5]:
          letterCorrect = letterCorrect + 1
          print('Correct!')
          tan.penup()
          tan.goto(28,-170)
          tan.pendown()
          tan.write(word[5], font = ('ariel', 16, 'normal'))
          tan.hideturtle()
      if len(word) > 6:
        if guess == word[6]:
          letterCorrect = letterCorrect + 1
          print('Correct!')
          tan.penup()
          tan.goto(53,-170)
          tan.pendown()
          tan.write(word[6], font = ('ariel', 16, 'normal'))
          tan.hideturtle()
      if len(word) > 7:
        if guess == word[7]:
          letterCorrect = letterCorrect + 1
          print('Correct!')
          tan.penup()
          tan.goto(78,-170)
          tan.pendown()
          tan.write(word[7], font = ('ariel', 16, 'normal'))
          tan.hideturtle()
      if len(word) > 8:
        if guess == word[8]:
          letterCorrect = letterCorrect + 1
          print('Correct!')
          tan.penup()
          tan.goto(103,-170)
          tan.pendown()
          tan.write(word[8], font = ('ariel', 16, 'normal'))
          tan.hideturtle()
      if len(word) > 9:
        if guess == word[9]:
          letterCorrect = letterCorrect + 1
          print('Correct!')
          tan.penup()
          tan.goto(128,-170)
          tan.pendown()
          tan.write(word[9], font = ('ariel', 16, 'normal'))
          tan.hideturtle()
      elif guess not in word:
        lives = lives + 1
        if (lives == 1):
          print('You selected \'' + guess + '\'. That is not one of the letters. Try Again!(1/7)')
          tan. penup()
          tan.goto(75,75)
          tan.pendown()
          tan.circle(25)
        if lives == 2:
          print('You selected '+ guess +'.That is not one of the letters. Try Again!(2/7)')
          tan.penup()
          tan. goto(75,75)
          tan.pendown()
          tan.right(90)
          tan.forward(50)
        if lives == 3:
          print('YOu selected ' + guess + '.That is not one of the letters. Try Again!(3/7)')
          tan.penup()
          tan.goto(75,25)
          tan.pendown()
          tan.goto(50,0)
        if lives == 4:
          print('You selected ' + guess + '. That is not one of the letters. Try Again!(4/7)')
          tan.penup()
          tan.goto(75,25)
          tan.pendown()
          tan.goto(100,0)
        if lives == 5:
          print('You selected ' + guess + '. That is not one of the letters. Try Again!(5/7)')
          tan.penup()
          tan.goto(75,50)
          tan.pendown()
          tan.goto(115,80)
        if lives == 6:
          print('You selected ' + guess + 'That is not one of the letters. Try Again!(6/7)')
          tan.penup()
          tan.goto(75,50)
          tan.pendown()
          tan.goto(35,80)
        if lives == 7:
          print('You selected ' + guess + '. That is not one of the letters. Oh no! You''re out of tries!')
          tan.penup()
          tan.goto(70, 90)
          tan.pendown()
          tan.circle(3)
          tan. penup()
          tan.goto(85, 105)
          tan.pendown()
          tan.circle(2)
          tan.penup()
          tan.goto(65,105)
          tan.pendown()
          tan.circle(2)

    playAgain = input('Would you like to play again? Yes or No.')
  print('Thanks for playing!')

#Tic Tac Toe Game
if gameSelect == 'Tic Tac Toe' or gameSelect == 'tic tac toe' or gameSelect == 'Tic tac toe':
  playagain = 'Yes'
  while playagain == 'YES' or playagain == 'Yes' or playagain == 'yes':
    #draws tic tac toe board.
    tan.reset()
    tan.speed(20)
    tan.penup()
    tan.goto(-100,-250)
    tan.pendown()
    tan.goto(-100,250)
    tan.penup()
    tan.goto(100,-250)
    tan.pendown()
    tan.goto(100,250)
    tan.penup()
    tan.goto(-300,-84)
    tan.pendown()
    tan.goto(300,-84)
    tan.penup()
    tan.goto(-300,84)
    tan.pendown()
    tan.goto(300,84)
    tan.penup()
    tan.goto(-215, 160)
    tan.color(220,220,220)
    #writes the feint numbers that help the user know which square is represented by which number.
    tan.write('1', font = ('ariel', 35, 'normal'))
    tan.goto(-15, 160)
    tan.write('2', font = ('ariel', 35, 'normal'))
    tan.goto(185, 160)
    tan.write('3', font = ('ariel', 35, 'normal'))
    tan.goto(-215, -5)
    tan.write('4', font = ('ariel', 35, 'normal'))
    tan.goto(-15, -5)
    tan.write('5', font = ('ariel', 35, 'normal'))
    tan.goto(185, -5)
    tan.write('6', font = ('ariel', 35, 'normal'))
    tan.goto(-215, -172)
    tan.write('7', font = ('ariel', 35, 'normal'))
    tan.goto(-15, -172)
    tan.write('8', font = ('ariel', 35, 'normal'))
    tan.goto(185, -172)
    tan.write('9', font = ('ariel', 35, 'normal'))
    tan.color('black')
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


    playerName = input('Enter a Username')
    tan.penup()
    tan.goto(-125,280)
    tan.pendown()
    tan.write(playerName + ' vs. CPU!', font = ('ariel', 30, 'normal'))
    print(playerName + ', you are "Xs"')
    #the following lists are used to scan the selected boxes and see if any of them match a winning combination. They also prevent the computer from selecting a box that the user has already previously selected.
    list1 = []
    list2 = []
    list5 = ['one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine']
    listAll = [[1,2,3],[1,4,7],[1,5,9],[4,5,6],[7,8,9],[2,5,8],[3,6,9],[1,5,9],[3,5,7],[3,2,1],[2,1,3],[3,1,2],
               [5,2,8],[9,5,1],[5,9,1],[1,9,5],[7,4,1],[1,7,4],[8,5,2],[2,8,5],[1,9,6],[9,6,1],[6,5,4],[5,4,6],
               [4,6,5],[7,9,8],[8,9,7],[9,8,7],[3,7,5],[7,5,3],[3,7,5],[7,3,5],[6,4,5],[1,9,5],[1,3,2]]
    attempts = 0
    win = 0
    while win != 3 and attempts < 6:
      cpu = 1
      answer = int(input('Select the number of the box where you want to place your "x"!'))
      win = 0
      attempts = attempts + 1
      #these if statements check to determine where to put the X based on the user's decision.
      if answer == 1:
        tan.penup()
        tan.goto(-215,160)
        characterX()
        list1.append(1)
        list5.remove('one')
      if answer == 2:
        tan.penup()
        tan.goto(0,160)
        characterX()
        list1.append(2)
        list5.remove('two')
      if answer == 3:
        tan.penup()
        tan.goto(185,160)
        characterX()
        list1.append(3)
        list5.remove('three')
      if answer == 4:
        tan.penup()
        tan.goto(-215,0)
        characterX()
        list1.append(4)
        list5.remove('four')
      if answer == 5:
        tan.penup()
        tan.goto(0,0)
        characterX()
        list1.append(5)
        list5.remove('five')
      if answer == 6:
        tan.penup()
        tan.goto(185,0)
        characterX()
        list1.append(6)
        list5.remove('six')
      if answer == 7:
        tan.penup()
        tan.goto(-215,-172)
        characterX()
        list1.append(7)
        list5.remove('seven')
      if answer == 8:
        tan.penup()
        tan.goto(0,-172)
        characterX()
        list1.append(8)
        list5.remove('eight')
      if answer == 9:
        tan.penup()
        tan.goto(215,-172)
        characterX()
        list1.append(9)
        list5.remove('nine')
      for i in range(0,len(listAll)):
        if win == 3 :
          break
        else :
          win = 0
          for j in range(0,len(list1)):
            if listAll[i][0] == list1[j] or listAll[i][1] == list1[j] or listAll[i][2] == list1[j] :
              win = win + 1
              if win == 3 :
                print("You win! Thanks for playing.")
                cpu = 0
                playagain = input('Would you like to play again? Answer Yes or No')
                break

      #this randomly selects a number and places an O in the box randomly selected by the computer.
      if cpu == 1 and attempts < 5:
        cpuAns = random.choice(list5)
        if cpuAns == 'one':
          tan.penup()
          tan.goto(-240,110)
          characterO()
          list2.append(1)
          list5.remove('one')
        if cpuAns == 'two':
          tan.penup()
          tan.goto(-25,110)
          characterO()
          list2.append(2)
          list5.remove('two')
        if cpuAns == 'three':
          tan.penup()
          tan.goto(160,110)
          characterO()
          list2.append(3)
          list5.remove('three')
        if cpuAns == 'four':
          tan.penup()
          tan.goto(-240,-50)
          characterO()
          list2.append(4)
          list5.remove('four')
        if cpuAns == 'five':
          tan.penup()
          tan.goto(-25,-50)
          characterO()
          list2.append(5)
          list5.remove('five')
        if cpuAns == 'six':
          tan.penup()
          tan.goto(160,-50)
          characterO()
          list2.append(6)
          list5.remove('six')
        if cpuAns == 'seven':
          tan.penup()
          tan.goto(-240,-222)
          characterO()
          list2.append(7)
          list5.remove('seven')
        if cpuAns == 'eight':
          tan.penup()
          tan.goto(-25,-222)
          characterO()
          list2.append(8)
          list5.remove('eight')
        if cpuAns == 'nine':
          tan.penup()
          tan.goto(160,-222)
          characterO()
          list2.append(9)
          list5.remove('nine')
        for i in range(0,len(listAll)):
          if win == 3:
            break
          else:
            win = 0
            for j in range(0,len(list2)):
              if listAll[i][0] == list2[j] or listAll[i][1] == list2[j] or listAll[i][2] == list2[j] :
                win = win + 1
                if win == 3 :
                  print("You win! Thanks for playing.")
                  cpu = 0
                  playagain = input('Would you like to play again? Answer Yes or No')
                  break
      print('Thanks for Playing!')
