#problem 2 simulating getting a part B)
simprob2 <- function(nreps) {
	count4 <- 0
	countLW <- 0 #counts last woman
	for( i in 1:nreps)
	{
		men <- 5
		woman <- 3
		groupSize <- 0 
		while(groupSize < 3 || men == 5 || woman == 3)
			{
			choice <- runif(1)
			if(choice < men/(men+woman)){
				men <- men - 1		
				} #picked a man
			else{
			woman <- woman -1
			}#choose woman
			groupSize <- groupSize + 1
			}
		if(groupSize == 4){
		count4 <- count4 + 1
		if(woman == 2){
		countLW <- countLW +1 
		}
		}
	 
	}
	return(countLW/count4)
}
