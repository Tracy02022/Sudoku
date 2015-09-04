
#ifndef __Sudoku__VisualDisplay__
#define __Sudoku__VisualDisplay__

#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "CSP.h"

#define ratio 0.4  //determine the image that shows in the system
#define domain_left_margin 0.01
#define domain_top_margin 0.01
#define cell_left_pixel 0.25
#define cell_top_pixel 0.15
#define BUTTON_NUM 8
#define BUTTON_OPT_NUM 3 //the number of buttons as ratio buttons

#define DEBUG 0 //for test purpose

#endif /* defined(__Sudoku__VisualDisplay__) */


using namespace cv;


/*Cell is a unit on the Sudoku table*/
struct Cell{
    int assignment; // 0 - not assigned yet, which will only show domain on this image
                    // 1-9 - assigned value
    Point position; // the top left corner of this cell
    Size cell_size; 
    
    vector<int> domain; //for un-assigned cell only£¬ which stores the domain
    
};


/*Buttons*/
struct Button{
    Mat org_img; //store the original image that has no click
    Mat hov_img; //store the image when the mouse hove on top of it
    Mat clk_img; //store the image when the mouse is clicking on it.
    int status; //store the status of the button that 0 - shows original image, 1 - shows hover image, 2 - shows clicked image
    Point top_let; //the position of top left corner of the image
    Point bot_rgh; //the position of bottom right corner of the image
    
};

struct GoButton : Button{
    Mat org_img_2; //as the go button is special that there are go and pause pattern on the button
    Mat hov_img_2;
    Mat clk_img_2;
    int type; //2 - go is on; 1- paus is on
};

/*The GUI class*/
class VisualDisplay
{
public:
    CSP csp_data; //the most important variable where the values of each cell are generated from
    Mat sudoku_img; //the final image showing to the user
    Mat sudoku_blank; //the blank image without showing anything on the display
    Mat num_img[9]; //store the number of images
    Mat num_blue_img[9]; //store the number of assignment which has blue value
    Cell cell[9][9];
    Mat open_bracket, close_bracket;
    int chosen_cell; //stores the index of the cell (this number is in row-wise for the cell)
                     //-1 indicates non-selection
    
    /*Workflow control image*/
    Button button[BUTTON_NUM]; //the buttons
    GoButton go_btn; //go button for running the algorithm in a batch
    bool goal_found; //an indicator telling whether the goal has been found. If so, disable all the buttons. 
    
    /*For debug use*/
    int num_steps;
    
public:
    VisualDisplay();
    ~VisualDisplay();
    void load(char* file_name);
    void run(); //the main function of the class that shows the sudocu image which is the main GUI
    void paintContainer(); //draw the number assignment and domain on the sudoku image.
    void paintButtons(); //this is called by paintContainer
    void assignDomainToCell(int cell_row, int cell_col); //set the domain to each cell
    void assignValueToCell(int cell_row, int cell_col); //set the value to each cell
    void renderCellDomain(int cell_row, int cell_col); //paint the domain of the cell on the image
    void renderCellValue(int cell_row, int cell_col);
    void selectCell(int x, int y);
    
    
    /*Button Check*/
    void buttonHoverCheck(int x, int y);
    void buttonClickCheck(int x, int y);
    
    /*Button Callback Functions*/
    void onNext();
    void onBack();
    void onGo();
    
    void refreshData(); //retrieve new data from the CSP
};
