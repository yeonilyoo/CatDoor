FILE(REMOVE_RECURSE
  "facedetect.pdb"
  "facedetect"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/facedetect.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
