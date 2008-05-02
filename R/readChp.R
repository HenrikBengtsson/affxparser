readChp <- function(filename, withQuant = TRUE)
	.Call("R_affx_get_chp_file", filename, withQuant)
