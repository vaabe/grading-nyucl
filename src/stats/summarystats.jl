using DelimitedFiles
using UnicodePlots
using Statistics

cd(ARGS[2])

filepath = ARGS[1]
gradesfile = readdlm(filepath, ',', skipstart = 2)
grades = gradesfile[2:size(gradesfile)[1], 5]
numstudents = size(grades)[1]
numgraded = 0

for i = 1:numstudents
	if typeof(grades[i]) == SubString{String}
		grades[i] = 99.9
	else
		grades[i] = convert(AbstractFloat, grades[i])
		global numgraded += 1
	end
end

numungraded = numstudents - numgraded
completegrades = Array{Float64}(undef, numgraded, 1)

j = 1
for i = 1:numstudents
	if grades[i] != 99.9
		completegrades[j] = grades[i]
		global j += 1
	end
end

completegrades = completegrades[1:numgraded, 1]

gmin = minimum(completegrades)
gmax = maximum(completegrades)
gmean = mean(completegrades)
gmedian = median(completegrades)
gstd = std(completegrades)

gfloor = floor(gmin)
gceil = ceil(gmax)

numbins = gceil - gfloor

hist = histogram(completegrades, 
				 nbins = numbins, 
				 closed = :right, 
				 border = :dotted, 
				 color = :yellow,
				 width = 40
				 )

println(hist)
println("")
println("   n_students:    ", numstudents)
println("   n_graded:      ", numgraded)
println("   n_ungraded:    ", numungraded)
println("")
println("   mean:          ", round(gmean; digits = 2))
println("   median:        ", round(gmedian; digits = 2))
println("   sd:            ", round(gstd; digits = 2))
println("   min:           ", round(gmin; digits = 2))
println("   max:           ", round(gmax; digits = 2))
