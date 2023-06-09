import cv2 # Import OpenCV
   
# read the image file
img = cv2.imread('D:\Kuliah Online\Tugas Akhir\software Robot\Ancabot\src\Mapping\DenahLapanganKRSRI2023.png')
   
ret, bw_img = cv2.threshold(img, 220, 255, cv2.THRESH_BINARY)
   
# converting to its binary form
bw = cv2.threshold(img, 240, 255, cv2.THRESH_BINARY)
  
# Display and save image 
cv2.imshow("Binary", bw_img)
cv2.imwrite("D:\Kuliah Online\Tugas Akhir\software Robot\Ancabot\src\Mapping\DenahLapanganKRSRI2023BW.png", bw_img)
cv2.waitKey(0)
cv2.destroyAllWindows()