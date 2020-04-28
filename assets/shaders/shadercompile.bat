glslc.exe fragment.frag -o fragment.spv
glslc.exe vertex.vert -o vertex.spv
glslc.exe raygen.rgen -o raygen.spv
glslc.exe miss.rmiss -o miss.spv
glslc.exe shadowMiss.rmiss -o shadowMiss.spv
glslc.exe closestHit.rchit -o closestHit.spv

glslc.exe intersection.rint -o intersection.spv
glslc.exe closestHitInt.rchit -o closestHitInt.spv
