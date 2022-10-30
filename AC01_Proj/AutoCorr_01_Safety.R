rm(list=ls())
print(Sys.time())

# library(tseries)
# library(forecast)
library(ggplot2)
library(pracma)

# For Linux (and Mac?) use //
# For windows use \\

output_file = "AutoCorrs_01_Safe.csv"
time_names = "sweep"  #legacy problem: sometimes it's quarter, other times it sweep
# Think I found a way around this: just rename the local frame to have sweep as the time column

seperator = "\\" # No need to adjust this, will be adjusted based on OS in the next line

if (print(Sys.info()['sysname']) == "Linux") {
  root_root = "~"
  seperator = "//"} 

rootpath = paste(root_root, "Coding", "Cpp", "DDS", "AutoCorrData", sep = seperator)
projectpath = paste(root_root, "Coding", "Cpp", "DDS", "AC01_Proj", sep = seperator)

auto_core <- function(inseries) {
  x <- c()
  
  tmax <- length(inseries) - 1
  
  alpha <- 1 / (tmax - 1)
  
  term1 <- 0
  term2 <- 0
  term3 <- 0
  
  for (tp in 1: (tmax - 1)) {
    term1 <- term1 + (inseries[tp] * inseries[tp + 1])
    term2 <- term2 + (inseries[tp])
    term3 <- term3 + (inseries[tp + 1])
  }
  
  x[1] <- (alpha*term1) - ((alpha*term2)*(alpha*term3))
  
  
  for (t in 2:(tmax)) {
    alpha <- 1 / (tmax - t)
    
    term1 <- 0
    term2 <- 0
    term3 <- 0
    
    for (tp in 2: (tmax - t)) {
      term1 <- term1 + (inseries[tp] * inseries[tp + t])
      term2 <- term2 + (inseries[tp])
      term3 <- term3 + (inseries[tp + t])
    }
    
    x[t] <- ((alpha*term1) - ((alpha*term2)*(alpha*term3))) / x[1]
    
    if (x[t] < 0) {break}
    if (t > 50000) {break}
    
  }
  
  return(x)
  
}



# Takes a T and a tmax and finds how many n samples need to be taken.
# T and tmax need to be series of equal length
measure_count <- function(tau_series, tmax_series) {
  nseries <- list()
  
  for (i in 1:length(tau_series)) {
    nseries[[i]] <- tmax_series[[i]] / (2*tau_series[[i]])
  }
  
  return(nseries)
  
}




# Folder to look in
# rootpath = "D:\\Coding\\Cpp\\IsingModel\\OtherFiles\\2_3_20000000"

rootfolder <- list.files(path = rootpath)

area01 <- list()
areamax <- list()
f_index <- list()
t_index <- list()
e_index <- list()
s_index <- list()

for (i in 1:length(rootfolder)) {
  
  subpath <- paste(rootpath, rootfolder[i], sep = seperator)
  subfolder <- list.files(path = subpath)
  
  master_frame <- data.frame()
  
  # Iterate through the folder and put all the data into the master frame
  for (j in 1:length(subfolder)) {
    filepath <- paste(subpath, subfolder[j], sep = seperator)
    # print(filepath)
    
    local_frame <- read.csv(filepath, header = TRUE)
    master_frame <- rbind(master_frame, local_frame)
  }
  
  master_frame <- master_frame[order(master_frame["sweep"]), ]
  rownames(master_frame) <- 1:nrow(master_frame)
  print("Loaded Master Frame")
  
  print("Begining Auto Correlation")
  sf01 <- auto_core(master_frame$SFk01)
  sfx <- rep(1:length(sf01) - 1)
  area01[[i]] <- trapz(unlist(sfx), unlist(sf01))
  areamax[[i]] <- length(rownames(master_frame))
  f_index[[i]] <- subpath
  
  file_information <- strsplit(filepath, "_")
  t_index <- file_information[[1]][2]
  e_index <- file_information[[1]][4]
  s_index <- file_information[[1]][6]
  
  print("Finished auto correlation of file: ")
  print(subpath)
  
}

sfn <- measure_count(area01, areamax)


output_data <- data.frame(Temp = t_index, E = e_index, Size = s_index, S01tau = unlist(area01), S01tmax = unlist(areamax), S01n = unlist(sfn))
rownames(output_data) <- f_index

write.csv(output_data, paste(projectpath, output_file, sep = seperator))

print(Sys.time())
