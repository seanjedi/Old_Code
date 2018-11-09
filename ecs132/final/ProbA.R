# returns list of pixel values from imgobj
getnonbound <- function(imgobj)
{
  # grab the original image height and width from dimensions
  height <- dim(getChannels(imgobj))[1]
  width  <- dim(getChannels(imgobj))[2]
  
  # grab the matrix of all greyscale values
  data <- imgobj@grey
  
  # remove the first and last columns and rows (border pixels)
  
  # have to remove last 1st, so index doesn't change
  data <- data[,-width]
  data <- data[,-1]
  
  # same as before, remove last row first
  data <- data[-height,]
  data <- data[-1,]
  
  # create the y vector (all interior pixels stored as a vector)
  y <- as.vector(data)
  
  # make the X matrix (grabbing N / S / E / W neighboring pixels)
  x <- matrix(nrow = length(y), ncol = 4)
  
  entries <- 1:nrow(x)  # the row numbers of the X matrix
  xVals <- 2:(width-1)  # the default x coordinates for N / S
  yVals <- 2:(height-1) # the default y coordinates for E / W
  
  x[entries, 1] <- imgobj@grey[1:(height-2), xVals] # North Values
  x[entries, 2] <- imgobj@grey[3:(height-0), xVals] # South Values
  x[entries, 3] <- imgobj@grey[yVals,  1:(width-2)] # East  Values
  x[entries, 4] <- imgobj@grey[yVals,  3:(width-0)] # West  Values
  
  list(y = y, x = x) # the return list
  
  # Old Method for grabbing neighboring pixel values
  # Runs in O((height - 2) * (width - 2))
  # takes a lot of extra time due to R loops
  # entry <- 1 # start iterator for row count
  # for(xval in 2:(width-1)) # iterate over only interior pixels
  # {
  #   # iterate on y-coordinates second so that the format
  #   # exactly matches that of the y vector (column-major)
  #   for(yval in 2:(height-1))
  #   {
  #     # yval is listed first (column-major)
  #     nsewMatrix[entry, 1] <- greyMatrix[yval - 1, xval] 
  #     nsewMatrix[entry, 2] <- greyMatrix[yval + 1, xval] 
  #     nsewMatrix[entry, 3] <- greyMatrix[yval, xval + 1]  
  #     nsewMatrix[entry, 4] <- greyMatrix[yval, xval - 1] 
  #     # move to the next row
  #     entry <- entry + 1
  #   }
  # }
}

noise <- function(imgname, p)
{
  require(pixmap) # nead to read the image in as a pixmap object
  
  image <- read.pnm(imgname)
  height <- dim(getChannels(image))[1]
  width  <- dim(getChannels(image))[2]
  
  # get a count of the total pixels in the image
  pixelCount <- height * width
  
  # randomly generate p proportion to generate noise on
  noisy <- sample(1:pixelCount, pixelCount * p, replace=FALSE)
  
  # y coordinates are the remainder after we fit our pixel number into the image height evenly as many times as possible
  yCoors <- (noisy %% height)
  # if 0, then are exactly divisible and yCoordinate is the bottom edge (height)
  yCoors <- ifelse(yCoors == 0, height, yCoors)
  
  # x coordinates are the number of times we can fit our pixel number into the image height evenly (+1)
  xCoors <- (noisy %/% height) + 1
  # only occurs if the bottom right corner is a selected pixel, and dividing returns us exactly the width of the image
  xCoors <- ifelse(xCoors > width, width, xCoors) 
  
  # convert x,y into a tuple and grab the corresponding index, replace each with random U(0, 1) values
  image@grey[cbind(yCoors, xCoors)] <- runif(pixelCount * p, 0, 1)
  
  write.pnm(image, paste("noised", imgname, sep="_"))
}

denoise <- function(imgname)
{
  require(pixmap) # nead to read the image in as a pixmap object
  
  # grab basic image details
  image <- read.pnm(imgname)
  height <- dim(getChannels(image))[1]
  width  <- dim(getChannels(image))[2]
  
  # construct the regression coefficients using nearest neighbor prediction method
  nonbound <- getnonbound(image)
  coeffs <- lm(nonbound$y ~ nonbound$x, nonbound)$coefficients
  
  # generate the predicted values (matrix multiply)
  prediction <- as.matrix(cbind(1, nonbound$x)) %*% coeffs 
  prediction <- ifelse(prediction < 0, 0, prediction) # if values are below 0, set them to 0 (to avoid wrap-around)
  prediction <- ifelse(prediction > 1, 1, prediction) # same for values above 1
  
  interiorX <- 2:(width-1)  # the interior x-coordinates
  interiorY <- 2:(height-1) # the interior y-coordinates
  
  image@grey[interiorY, interiorX] <- prediction # apply the prediction values to all interior pixels
  
  write.pnm(image, paste("de", imgname, sep=""))
}

tester <- function(imageNames, p)
{
  for(name in imageNames)
  {
    noise(name, p)
    denoise(paste("noised", name, sep="_"))
  }
}

getCoeffs <- function(imgname)
{
  require(pixmap) # nead to read the image in as a pixmap object
  
  # grab basic image details
  image <- read.pnm(imgname)
  
  # construct the regression coefficients using nearest neighbor prediction method
  nonbound <- getnonbound(image)
  coeffs <- lm(nonbound$y ~ nonbound$x, nonbound)$coefficients
  print(coeffs)
}
