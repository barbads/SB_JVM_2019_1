class Rectangle 
{ 
     int length; //length of rectangle 
     int breadth; //breadth of rectangle 
     static int rectCount =0; //count rectang objects 
   
    void setData(int l,int b) 
     { 
      
        length=l; 
        breadth=b; 
        rectCount++; 
     } 
     //method to calculate area of rectangle 
     int area() 
     { 
        int rectArea; 
        rectArea = length * breadth; 
        return rectArea; 
        
     } 
} 