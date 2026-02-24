target_sources(stonewall PUBLIC
	stonewall.cpp meshIO.cpp libnoisetool.cpp noiseutils.cpp miniz.c mesh.cpp
)

target_include_directories(stonewall PRIVATE ".")
