
for (zzz in 0) {

# Scan current directory for CEL files
celFiles <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
if (length(celFiles) == 0)
  break

units <- 100:261
t <- system.time({
cel <- readCelUnits(celFiles, units=units, readPixels=TRUE, readStdvs=TRUE)
})
print(cel[1:2])

cat(sprintf("Time per unit and array: %.4fms\n", 
                                   1000*t[3]/length(cel)/length(celFiles)))

# Clean up
rm(celFiles, cel, t)

} # for (zzz in 0)
