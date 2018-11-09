# wrap all problem B into one function
probB <- function()
{
  data <- getTrainingAndTest(getData())
  meanAnalyze(data$training, data$test)
  print(regressionAnalyze(data$training, data$test))
  print(logisticAnalyze(data$training, data$test))
  principalAnalyze(data$training, data$test)
}

# reads the data in from the .txt file
getData <- function()
{
  require(data.table)
  
  #We use fread to read in the file "year Prediction MSD.txt" since it is significantly faster than read.csv
  #Using fread however does put our data into a datatable instead of a data.frame, limiting us on what functions we can run on our data
  fread("YearPredictionMSD.txt")
}

# get the training and test data sets from all data
getTrainingAndTest <- function(data)
{
  #Now we need to split our data into two parts: training set, and test set
  #We need training set to hold 2/3rds of the data, and test to hold the rest
  set.seed(101) # just a random seed to ensure we get random data
  sample <- sample.int(nrow(data), floor(2/3*nrow(data)), replace = F)
  training <- data[sample, ]
  test <- data[-sample, ]
  
  list(training = training, test = test)
}

#COMPARISON OF MEANS
meanAnalyze <- function(training, test)
{
  #We now need to split the training set into two parts
  #one part with the year of the data to be before 1996, and then the rest
  L <- (training$V1 < 1996) # a logical index vector
  
  trainingSplitPre1996 <- training[L,]   # all training data before 1996 (pre- autotune)
  trainingSplitPost1996 <- training[!L,] # all training data after  1996 (post-autotune)
  
  # we need to compare the value of v77 in both pre-1996 and post-1996 via means
  trainingSplitMeanComparisonResults <- t.test(trainingSplitPre1996[,77], trainingSplitPost1996[,77])
  print(trainingSplitMeanComparisonResults)
}

# find the mean square error for using ALL variables in the training set
regressionAnalyze <- function(training, test)
{
  coeffs <- lm(V1 ~ . , training)$coefficients
  computeAverageSquaredPredictionError(test, coeffs)
}

logisticAnalyze <- function(training, test)
{
  # Fit logistic model
  newFirstCol <- ifelse(training[,1] >= 1996, 1, 0)
  g <- glm(newFirstCol ~ ., data=training[,-1], family=binomial)
  
  # Now, we can find the predicted values, remembering that
  # we're dealing with a logistic function
  betasSum <- as.matrix(cbind(1,test[,2:ncol(test)])) %*% g$coefficients
  predictions <- 1 / (1 + exp(-1 * betasSum))
  
  # Find proportion of incorrectly predicted cases. More specifically,
  # among all rows we predicted would have a year of at least 1996,
  # find the proportion that are before 1996.
  predictedClassifications <- (predictions > 0.5)   # True/false values
  mean(as.matrix(test)[predictedClassifications,1] < 1996)
}

principalAnalyze <- function(training, test)
{
  require(ggplot2)
  
  P <- prcomp(training[,-1])$rotation # principal components matrix
  
  # get the components from the training matrix and test matrix
  components <- as.matrix(training[,2:ncol(training)]) %*% P
  testComponents <- as.matrix(test[,2:ncol(test)]) %*% P
  
  # so the computeAvSqPrErr function works
  testComponents <- cbind(test[,1], testComponents)
  
  # establish first colum of data as the song years
  data <- as.data.frame(components)
  errors <- c() # to store errors as we process them
  
  # this takes a really long time to run, because it is not optimized
  # but I am unsure of how to optimize it any better.  We have to do the lm 90 times
  for(i in 1:ncol(components))
  {
    # find the mean square errors for each component
    PredictorVariables <- paste("components[,", 1:i, "]", sep="")
    Formula <- formula(paste("training$V1 ~ ", paste(PredictorVariables, collapse=" + ")))
    coeffs <- lm(Formula, data)$coefficients
    errors <- append(errors, computeAverageSquaredPredictionError(testComponents[,1:(i+1)], coeffs))
  }
  
  # now plot the results
  plotData <- data.frame(components = 1:ncol(components), meanSquareErrors = errors)
  plot <- ggplot(plotData, aes(x=components, y=meanSquareErrors)) + geom_line()
  plot + labs(title="Principal Component Analysis", x = "Number of Principal Components", y = "Mean Squared Error") 
}

# returns the mean squared error between the test set and a test set
computeAverageSquaredPredictionError <- function(test, coeffs)
{
  # grab the predicted values using matrix multiplication
  predictions <- as.matrix(cbind(1, test[,-1])) %*% coeffs
  # Then compute average squared prediction error over all the test set.
  meansSquared <- mean((predictions - test[,1])^2)
}
