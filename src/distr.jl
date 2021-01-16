using DelimitedFiles
using UnicodePlots

gradefile = readdlm(string(ARGS[1], "/grades.csv"), ',')
grades = gradefile[2:size(gradefile)[1], 5]
grades = convert(Array{Float64}, grades)

distrib = histogram(grades, nbins = 5, closed = :right)
println(distrib)
