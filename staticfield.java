class Rectangle 
{ 
     static int length; //length of rectangle 
     static int breadth; //breadth of rectangle 
     static int rectCount = 0; //count rectang objects 
     
     void setData(int l,int b) 
     { 
      
        length=l; 
        breadth=b; 
        rectCount++; 
     } 
     //method to calculate area of rectangle 
     static int area() 
     { 
        int rectArea; 
        rectArea = length * breadth; 
        return rectArea;  
     } 
    
     public static void main(String args[]){  
            Rectangle obj = new Rectangle();  
            obj.setData(3, 5);
            int area = obj.area();
            System.out.println(area);
     }
}
