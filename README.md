# OpenGL_volume_rendering
Simple box-volume rendering

## Read density voxel from text file

  ```
  min 1 1 1 : defines min position of volume box
  
  max 5 5 5 : defines max position of volume box
  
  num 4 4 4 : defines number of sub voxel in each axis
  
  s 1 0.5 0.3 : defines scattering paramerter
  
  a 0.3 0.3 0.3 : defines absorption paramerter
  
  d 1 : defines density parameter
  
  If Each voxel has difference property, declar s,a and d for every voxel, order in axis x-y-z.
      s # # # 
      a # # # 
      d #
      s # # # 
      a # # # 
      d # 
  Else, declare s and a just once.
    s # # #
    a # # # 
    d #
    d #
    ...
 ```
 
 ![box_volume_rendering](https://user-images.githubusercontent.com/60053627/220272468-59c1b455-f01e-4e07-8626-ff1007f43442.png)
