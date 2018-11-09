# --------------------------------------------------------------------------- #
# Authors: joelewis, spmalloy, apksingh   @ ucdavis . edu                     #
# --------------------------------------------------------------------------- #
# Problem 2a: find the long-run proportion of games we win                    #
# Note that we can model this game as a negative binomial family with the     #
# adjustment that was made (accumulate 3 heads).  We can no longer model the  #
# game using the Markov model because it is now no longer true that the       #
# probability of advancing to one state from a given state does not depend on #
# how we got to the given state.  Here, indeed, we see that our chances of    #
# moving to state 0 (restarting the game with 0 heads accumulated) from any   #
# given state is different based on the total number of tosses we have        #
# performed up to this point.  So we must now use the negative binomial family#
# to model this game.  Here we see the variable N defined as the number of    #
# trials required to achieve the 3rd success (3 heads total).  So we can      #
# discover our chances of winning the game as the probability that we achieve #
# Do not forget that the negative binomial distribution is defined for the R  #
# language as the number of failures before the k-th success, not the total   #
# number of trials as we define it in our book.  So we must subtract the      #
# success numbers from our total tosses to get the failure count for pnbinom  #
# 3 successes at or before 10 total tosses (7 failures):                      #
# So P(win the game) = P(N <= 10)                                             #
#                    = pnbinom(10 - 3, 3, 0.5)                                #
#                    = 0.9453                                                 #
# --------------------------------------------------------------------------- #
# Problem 2b: find the long-run average of our winnings                       #
# Let us model our winnings by W = 11 - N if we win the game, and W = 0       #
# if we lose.  Then we can find the expected value of winnings as follows:    #
# E(W) = P(win the game) * E(11 - N) + P(lose the game) * E(0)                #
#      = 0.9453 * (11 - E(N))                                                 #
#      = 0.9453 * (11 - 5.628)                                                #
#      = 0.9453 * (5.372)                                                     #
#      = 5.08                                                                 #
# So we are expected to win $5.08 per game in the long run.  Note that we     #
# cannot actually win this amount in any one game.                            #
# --------------------------------------------------------------------------- #
# Problem 2c: find the expected value of the number of tosses required to win #
# We will use the following value for our calculations in part 2b as well,    #
# so that math that follows verifies that section.  Note that we cannot simply#
# use the expected value of a standard negative binomial distribution because #
# our game ends after 10 tosses wether we win or not, so we must consider the #
# expected value to be relative to the possible ways in which we can win.     #
# E(N) = P(N = 3 | win the game) * 3 + P(N = 4 | win the game) * 4 + ...      #
#      = summation from i = 3 to i = 10 (P(N = i) * i) / P(win the game)      #
#      = summation from i = 3 to i = 10 (dnbinom(i - 3, 3, 0.5) * i) / P(win) #
#      = 5.320 / 0.9453                                                       #
#      = 5.628                                                                #
# Note that we can make the leap from line 1 to line 2 because:               #
# P(N = 3 | win) = P(win and N = 3) / P(win)                                  #
# P(win and N = 3) = P(N = 3) * P(win | N = 3)                                #
# note that we cannot lose if N = 3, so:                                      #
# P(win | N = 3) = 1 --> P(win and N = 3) = P(N = 3)                          #
# P(N = 3 | win) = P(N = 3) / P(win)                                          #
# Then the distributive property allows us to simply divide the sum of all    #
# these P(N = i) by the P(win), as long as each i is a guaranteed winning case#
# (which they are from 3 --> 10).                                             #
# --------------------------------------------------------------------------- #
# These expressions are verified through R simulation code below              #
# --------------------------------------------------------------------------- #

#Problem 2: simulate the 3-heads-in-a-row game with the variant -- accumulate 3 heads
sim<-function(ngames)
{
  results <- c()  # the list of a) the long-run proportion of games we win
                  #             b) the long-run average winnings per game
                  #             c) the expected value of coin flips needed to win
  
  gamesWon <- 0
  gameWinnings <- c()
  gameTosses <- c()
  
  for(i in 1:ngames)
  {
    numTosses <- 0
    numHeads <- 0
    
    while(numTosses < 10 && numHeads < 3)
    {
      if(runif(1) < 0.5) numHeads <- numHeads + 1
      numTosses <- numTosses + 1
    }
    
    gamesWon <- ifelse(numHeads == 3, gamesWon + 1, gamesWon)
    gameWinnings <- append(gameWinnings, ifelse(numHeads == 3, 11 - numTosses, 0))
    gameTosses <- append(gameTosses, ifelse(numHeads == 3, numTosses, 0))
  }
  
  results <- append(results, gamesWon / ngames)           # the proportion of games we won
  results <- append(results, sum(gameWinnings) / ngames)  # the long-run average winnings
  results <- append(results, sum(gameTosses) / gamesWon)  # the number of flips required to win a game
  
  return(results)
}

# Simulation code based on section 6.6 example but updated for this game (3 total heads)
# only because the blog says our code should look like the 6.6 example...
# the normal simulation code above is MUCH more understandable and more like the actual
# process going on in the game (since we aren't actually running a Markov chain)
# but this simulation code does actually run a lot faster
simExampleBased<-function(ngames)
{
  results <- c()
  
  gamesPlayed <- 0
  nWins <- 0
  winTimes <- 0
  numHeads <- 0
  numTosses <- 0
  startPlay <- 0
  timeStep <- 0
  while(gamesPlayed < ngames)
  {
    if(runif(1) < 0.5) numHeads <- numHeads + 1
    numTosses <- numTosses + 1
    
    if(numHeads == 3)
    {
      winTimes <- winTimes + timeStep - startPlay
      nWins <- nWins + 1
      startPlay <- timeStep
      gamesPlayed <- gamesPlayed + 1
      numHeads <- 0
      numTosses <- 0
    }
    else if(numTosses == 10)
    {
      gamesPlayed <- gamesPlayed + 1
      startPlay <- timeStep
      numHeads <- 0
      numTosses <- 0
    }
    
    timeStep <- timeStep + 1
  }
  
  results <- append(results, nWins / ngames)
  results <- append(results, (nWins / ngames) * (11 - (winTimes / nWins)))
  results <- append(results, winTimes / nWins)
  
  return(results)
}
