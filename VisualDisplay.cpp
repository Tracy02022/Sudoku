#include "VisualDisplay.h"

/*Test Case*/
int test_data[81] = {3, 7, 9, 8, 2, 1, 4, 5, 6,
    5, 4, 8, 9, 3, 6, 7, 2, 1,
    2, 6, 1, 4, 5, 7, 3, 9, 8,
    1, 8, 5, 3, 6, 9, 2, 4, 7,
    9, 2, 4, 1, 7, 5, 8, 6, 3,
    6, 3, 7, 2, 8, 4, 9, 1, 5,
    4, 9, 3, 5, 1, 8, 6, 7, 2,
    8, 5, 6, 7, 9, 2, 1, 3, 4,
    7, 1, 2, 6, 4, 3, 5, 8, 9};


void onMouse( int event, int x, int y, int flags, void* param )
{
    VisualDisplay* vd = (VisualDisplay*) param;
    
    
    
    switch( event )
    {
        case CV_EVENT_LBUTTONDOWN:
            vd->selectCell(x, y);
            vd->buttonClickCheck(x, y);
            break;
            
        case CV_EVENT_RBUTTONDOWN:
            break;
        case CV_EVENT_LBUTTONUP:
            
            break;
            
            
        case CV_EVENT_RBUTTONUP:
            vd->chosen_cell = -1;
            
            break;
    
        case CV_EVENT_MOUSEMOVE:
            vd->buttonHoverCheck(x, y);
            break;
    }
    
}

void VisualDisplay::buttonHoverCheck(int x, int y)
{
    for(int i = 0; i < BUTTON_NUM; i++)
    {
       if(i < BUTTON_OPT_NUM) /*Radio Button Use*/
       {
           if(button[i].status != 2)
           {
               if(button[i].top_let.x < x && x < button[i].bot_rgh.x && button[i].top_let.y < y && y < button[i].bot_rgh.y)
               {
                   button[i].status = 1;
               }
               else
               {
                   button[i].status = 0;
               }
           }
       }
        else if(i == BUTTON_OPT_NUM || i == BUTTON_OPT_NUM + 1 || i == BUTTON_OPT_NUM + 2) /*For checkbox use (random selection of domain value*/
        {
            
        }
        else/*Next, Back Button use*/
        {
            if(button[i].top_let.x < x && x < button[i].bot_rgh.x && button[i].top_let.y < y && y < button[i].bot_rgh.y)
            {
                button[i].status = 1;
            }
            else
            {
                button[i].status = 0;
            }

        }
    }
    
    if(go_btn.top_let.x < x && x < go_btn.bot_rgh.x && go_btn.top_let.y < y && y < go_btn.bot_rgh.y)
    {
        go_btn.status = 1;
    }
    else
    {
        go_btn.status = 0;
    }
    
}

void VisualDisplay::buttonClickCheck(int x, int y)
{
    /* For option buttion (radio)*/
    for(int i = 0; i < BUTTON_OPT_NUM; i++)
    {
        if(button[i].status == 1)
        {
            button[i].status = 2;
            csp_data.alg_opt = i + 1;
            
            /*Set the rest option button to 0*/
            for(int j = 0; j < BUTTON_OPT_NUM; j++)
            {
                if(j != i)
                {
                    button[j].status = 0;
                }
            }
            
            /*Clear all the search data in CSP to have a new start*/
            csp_data.clearData();
            
            /*Reset the Go Button*/
            go_btn.type = 1; //initially it is in ready to go state.
            goal_found = 0; //initilly set goal is not found
            goal_found = false; //make sure all the buttons are clickable
        }
        
    }
    
    /* For check box use*/
    if(button[BUTTON_OPT_NUM].top_let.x < x && x < button[BUTTON_OPT_NUM].bot_rgh.x && button[BUTTON_OPT_NUM].top_let.y < y && y < button[BUTTON_OPT_NUM].bot_rgh.y)
    {
        button[BUTTON_OPT_NUM].status = (button[BUTTON_OPT_NUM].status == 0)? 2 : 0;
        if(button[BUTTON_OPT_NUM].status == 2)
        {
            /*Re-shuffle the domain of each variable*/
            csp_data.reshuffleDomain();
            csp_data.random = 1;
        }
        else{
            /*Sort the domain to sequentially increasing order*/
            csp_data.sortDomain();
            csp_data.random = 0;
        }
    }
    else if(button[BUTTON_OPT_NUM + 1].top_let.x < x && x < button[BUTTON_OPT_NUM + 1].bot_rgh.x && button[BUTTON_OPT_NUM + 1].top_let.y < y && y < button[BUTTON_OPT_NUM + 1].bot_rgh.y)
    {
        button[BUTTON_OPT_NUM + 1].status = (button[BUTTON_OPT_NUM + 1].status == 0)? 2 : 0;
    }
    else if(button[BUTTON_OPT_NUM + 2].top_let.x < x && x < button[BUTTON_OPT_NUM + 2].bot_rgh.x && button[BUTTON_OPT_NUM + 2].top_let.y < y && y < button[BUTTON_OPT_NUM + 2].bot_rgh.y)
    {
        button[BUTTON_OPT_NUM + 2].status = (button[BUTTON_OPT_NUM + 2].status == 0)? 2 : 0;
        
                
    }

    
    /* For controlling buttons: back, next */
    for(int i = BUTTON_OPT_NUM + 3; i < (BUTTON_NUM); i++)
    {
      if(button[i].status == 1)
      {
          button[i].status = 2;
          if(i == BUTTON_OPT_NUM + 3)
          {
              
              onNext();
          }
          if(i == BUTTON_OPT_NUM + 4)
          {
              onBack();
          }
      }
    }
    
    /* For the go button */
    if(go_btn.status == 1 || go_btn.status == 2)
    {
        go_btn.status = 2;
        go_btn.type = (go_btn.type == 1)?2:1;
        onGo();
    }
}

void VisualDisplay::selectCell(int x, int y)
{
    chosen_cell = -1;
    for(int j = 0; j < 9; j++)
    {
        for(int i = 0; i < 9; i++)
        {
            int left = cell[j][i].position.x;
            int right = cell[j][i].position.x + cell[j][i].cell_size.width;
            int top = cell[j][i].position.y;
            int bottom = cell[j][i].position.y + cell[j][i].cell_size.height;
            if(left <= x && x <= right && top <= y && y <= bottom)
            {
                chosen_cell = j * 9 + i;
                break;
            }
        }
        
        if(chosen_cell != -1)
            break;
    }
}

void VisualDisplay::onBack()
{
    goal_found = csp_data.goBack(&chosen_cell);
}

void VisualDisplay::onNext()
{
  
    if(goal_found != true)
    {
        if(csp_data.alg_opt == 1)
        {
            goal_found = csp_data.backTrack(&chosen_cell);
        }
        else if(csp_data.alg_opt == 2)
        {
            if(button[BUTTON_OPT_NUM + 2].status == 0)
                goal_found = csp_data.forwardChecking(&chosen_cell);
            else
                goal_found = csp_data.forwardCheckingOrder(&chosen_cell);
        }
        else if(csp_data.alg_opt == 3)
        {
            if(button[BUTTON_OPT_NUM + 2].status == 0)
                goal_found = csp_data.arcChecking(&chosen_cell);
            else
                goal_found = csp_data.arcCheckingOrder(&chosen_cell);
        }
    }
    
}

void VisualDisplay::onGo()
{
    
    if(goal_found != true)
    {
        if(csp_data.alg_opt == 1)
        {
            goal_found = csp_data.backTrack(&chosen_cell);
        }
        else if(csp_data.alg_opt == 2)
        {
            if(button[BUTTON_OPT_NUM + 2].status == 0)
                goal_found = csp_data.forwardChecking(&chosen_cell);
            else
                goal_found = csp_data.forwardCheckingOrder(&chosen_cell);
        }
        else if(csp_data.alg_opt == 3)
        {
            if(button[BUTTON_OPT_NUM + 2].status == 0)
                goal_found = csp_data.arcChecking(&chosen_cell);
            else
                goal_found = csp_data.arcCheckingOrder(&chosen_cell);
        }
        cv::waitKey(1);
    }
    
    
}

void VisualDisplay::refreshData()
{
    /*Get the assignment, domain, and status from csp_data*/
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            cell[y][x].assignment = csp_data.cur_state.values[y][x];
           // cell[y][x].domain.clear();
            cell[y][x].domain = csp_data.variables[y][x].domain;
        }
    }
}

VisualDisplay::VisualDisplay()
{
    cv::namedWindow("Sudoku");
    setMouseCallback("Sudoku", onMouse, this);
    
    /*The pixel position for each row (the top boundary)*/
    int row_pixel[9] = {15, 188, 368, 554, 729, 908, 1092, 1264, 1446};
    
    /*The pixel position for each column (the left boundary)*/
    int col_pixel[9] = {16, 190, 370, 554, 729, 908, 1092, 1268, 1448};
    
    
    /*Initialize all the cells*/
    for(int j = 0; j < 9; j++)
    {
        for(int i = 0; i < 9; i++)
        {
            cell[j][i].assignment = csp_data.cur_state.values[j][i];
            cell[j][i].position = Point(col_pixel[i] * ratio, row_pixel[j] * ratio);
           
            
            /*Initial the domain for each variable*/
            for(int idx = 0; idx < csp_data.variables[j][i].domain.size(); idx++)
            {
                cell[j][i].domain.push_back(csp_data.variables[j][i].domain[idx]);
            }
            
            /*Initial the cell location on the image*/
            if(i < 8 && j < 8)
            {
                cell[j][i].cell_size.width = (col_pixel[i + 1] - col_pixel[i]) * ratio;
                cell[j][i].cell_size.height = (row_pixel[j + 1] - row_pixel[j]) * ratio;
            }
            else
            {
                cell[j][i].cell_size.width = cell[0][0].cell_size.width;
                cell[j][i].cell_size.height = cell[0][0].cell_size.height;
            }
            
        }
    }
    
    /*Initially no cell has been selected*/
    chosen_cell = -1;
    
    /*Initiate the button status*/
    for(int i = 1; i < BUTTON_NUM; i++)
    {
        button[i].status = 0;
    }
    button[0].status = 2;
    go_btn.type = 1; //initially it is in ready to go state.
    goal_found = 0; //initilly set goal is not found
    
   // csp_data.setData(test_data);
}


VisualDisplay::~VisualDisplay(void)
{
}



/*Load all the images from hard drive*/
void VisualDisplay::load(char *file_name)
{
    sudoku_blank = imread(file_name); //load the blank image
    resize(sudoku_blank, sudoku_blank, cv::Size(sudoku_blank.cols * ratio, sudoku_blank.rows * ratio));
    
    /*Load the number image*/
    for(int i = 0; i < 9; i++)
    {
        char file_name[200];
        sprintf(file_name, "imgs/%d.png", i+1);
        num_img[i] = imread(file_name);
        resize(num_img[i], num_img[i], Size(num_img[i].cols * ratio, num_img[i].rows * ratio));
        
        sprintf(file_name, "imgs/%d_blue.png", i+1);
        num_blue_img[i] = imread(file_name);
        resize(num_blue_img[i], num_blue_img[i], Size(num_blue_img[i].cols * ratio, num_blue_img[i].rows * ratio));
    }
    
    /*Load the bracket image*/
    open_bracket = imread("imgs/open_bracket.png");
    close_bracket = imread("imgs/close_bracket.png");
    resize(open_bracket, open_bracket, Size(open_bracket.cols * ratio, open_bracket.rows * ratio));
    resize(close_bracket, close_bracket, Size(close_bracket.cols * ratio, close_bracket.rows * ratio));
    
    
    /*Load the button images*/
    button[0].org_img = imread("imgs/radio_button.png"); //button  0 (backtrack)
    button[0].hov_img = imread("imgs/radio_hover.png");
    button[0].clk_img = imread("imgs/radio_select.png");
    resize(button[0].org_img, button[0].org_img, Size(20, 20));
    resize(button[0].hov_img, button[0].hov_img, Size(20, 20));
    resize(button[0].clk_img, button[0].clk_img, Size(20, 20));
    button[0].top_let.x = sudoku_blank.cols + 50;
    button[0].top_let.y = sudoku_blank.rows * .15;
    button[0].bot_rgh.x = button[0].top_let.x + button[0].org_img.cols;
    button[0].bot_rgh.y = button[0].top_let.y + button[0].org_img.rows;

    
    button[0].org_img.copyTo(button[1].org_img); //button 1 (forward check)
    button[0].hov_img.copyTo(button[1].hov_img);
    button[0].clk_img.copyTo(button[1].clk_img);
    button[1].top_let.x = sudoku_blank.cols + 50;
    button[1].top_let.y = button[0].top_let.y + 40;
    button[1].bot_rgh.x = button[1].top_let.x + button[1].org_img.cols;
    button[1].bot_rgh.y = button[1].top_let.y + button[1].org_img.rows;
    

    button[0].org_img.copyTo(button[2].org_img); //button 2 (arc consistency)
    button[0].hov_img.copyTo(button[2].hov_img);
    button[0].clk_img.copyTo(button[2].clk_img);
    button[2].top_let.x = sudoku_blank.cols + 50;
    button[2].top_let.y = button[1].top_let.y + 40;
    button[2].bot_rgh.x = button[2].top_let.x + button[2].org_img.cols;
    button[2].bot_rgh.y = button[2].top_let.y + button[2].org_img.rows;

    
    button[3].org_img = imread("imgs/checkbox_unchecked.png"); //random check button
    button[3].hov_img = imread("imgs/checkbox_unchecked.png");
    button[3].clk_img = imread("imgs/checkbox_checked.png");
    resize(button[3].org_img, button[3].org_img, Size(20, 20));
    resize(button[3].hov_img, button[3].hov_img, Size(20, 20));
    resize(button[3].clk_img, button[3].clk_img, Size(20, 20));
    button[3].top_let.x = sudoku_blank.cols + 70; //render the check button
    button[3].top_let.y = button[2].top_let.y + 50;
    button[3].bot_rgh.x = button[3].top_let.x + button[3].org_img.cols;
    button[3].bot_rgh.y = button[3].top_let.y + button[3].org_img.rows;
    
    
    button[4].org_img = imread("imgs/checkbox_unchecked.png"); //domain show check button
    button[4].hov_img = imread("imgs/checkbox_unchecked.png");
    button[4].clk_img = imread("imgs/checkbox_checked.png");
    resize(button[4].org_img, button[4].org_img, Size(20, 20));
    resize(button[4].hov_img, button[4].hov_img, Size(20, 20));
    resize(button[4].clk_img, button[4].clk_img, Size(20, 20));
    button[4].top_let.x = sudoku_blank.cols + 70; //render the check button
    button[4].top_let.y = button[3].top_let.y + 30;
    button[4].bot_rgh.x = button[4].top_let.x + button[4].org_img.cols;
    button[4].bot_rgh.y = button[4].top_let.y + button[4].org_img.rows;
    
    button[5].org_img = imread("imgs/checkbox_unchecked.png"); //ordering check button
    button[5].hov_img = imread("imgs/checkbox_unchecked.png");
    button[5].clk_img = imread("imgs/checkbox_checked.png");
    resize(button[5].org_img, button[5].org_img, Size(20, 20));
    resize(button[5].hov_img, button[5].hov_img, Size(20, 20));
    resize(button[5].clk_img, button[5].clk_img, Size(20, 20));
    button[5].top_let.x = sudoku_blank.cols + 70; //render the check button
    button[5].top_let.y = button[4].top_let.y + 30;
    button[5].bot_rgh.x = button[5].top_let.x + button[5].org_img.cols;
    button[5].bot_rgh.y = button[5].top_let.y + button[5].org_img.rows;
    
    
    button[6].org_img = imread("imgs/next.png"); //next button (move one step forward)
    button[6].hov_img = imread("imgs/next_hover.png");
    button[6].clk_img = imread("imgs/next.png");
    resize(button[6].org_img, button[6].org_img, Size(80, 40));
    resize(button[6].hov_img, button[6].hov_img, Size(80, 40));
    resize(button[6].clk_img, button[6].clk_img, Size(80, 40));
    button[6].top_let.x = sudoku_blank.cols + 50; //render the next button
    button[6].top_let.y = button[5].top_let.y + 40;
    button[6].bot_rgh.x = button[6].top_let.x + button[6].org_img.cols;
    button[6].bot_rgh.y = button[6].top_let.y + button[6].org_img.rows;
    
    button[7].org_img = imread("imgs/back.png"); //back button  (move one step backward)
    button[7].hov_img = imread("imgs/back_hover.png");
    button[7].clk_img = imread("imgs/back.png");
    resize(button[7].org_img, button[7].org_img, Size(80, 40));
    resize(button[7].hov_img, button[7].hov_img, Size(80, 40));
    resize(button[7].clk_img, button[7].clk_img, Size(80, 40));
    button[7].top_let.x = sudoku_blank.cols + 160; //render the next button
    button[7].top_let.y = button[6].top_let.y;
    button[7].bot_rgh.x = button[7].top_let.x + button[7].org_img.cols;
    button[7].bot_rgh.y = button[7].top_let.y + button[7].org_img.rows;
    
    
    go_btn.org_img = imread("imgs/go.png"); //next button 0 (move one step forward)
    go_btn.hov_img = imread("imgs/go_hover.png");
    go_btn.clk_img = imread("imgs/pause_hover.png");
    resize(go_btn.org_img, go_btn.org_img, Size(80, 80));
    resize(go_btn.hov_img, go_btn.hov_img, Size(80, 80));
    resize(go_btn.clk_img, go_btn.clk_img, Size(80, 80));
    go_btn.org_img_2 = imread("imgs/pause.png"); //next button 0 (move one step forward)
    go_btn.hov_img_2 = imread("imgs/pause_hover.png");
    go_btn.clk_img_2 = imread("imgs/go_hover.png");
    resize(go_btn.org_img_2, go_btn.org_img_2, Size(80, 80));
    resize(go_btn.hov_img_2, go_btn.hov_img_2, Size(80, 80));
    resize(go_btn.clk_img_2, go_btn.clk_img_2, Size(80, 80));
    go_btn.top_let.x = sudoku_blank.cols + 110; //render the next button
    go_btn.top_let.y = button[4].top_let.y + 130;
    go_btn.bot_rgh.x = go_btn.top_let.x + go_btn.org_img.cols;
    go_btn.bot_rgh.y = go_btn.top_let.y + go_btn.org_img.rows;

    
}

/*Render the cell that has not been assigned yet which has only domain available*/
void VisualDisplay::renderCellDomain(int cell_row, int cell_col)
{
    int num_height = num_img[0].rows / 3;
    int num_width = num_img[0].cols / 3;
    int gap = (5.0/0.3) * ratio;

    /*Render the brackets*/
    int top_margin = (cell[cell_row][cell_col].cell_size.height - open_bracket.rows)/2;
    
    int close_bracket_x = cell[cell_row][cell_col].position.x + cell[cell_row][cell_col].cell_size.width - close_bracket.cols - gap;
    
    open_bracket.copyTo(sudoku_img(cv::Range(cell[cell_row][cell_col].position.y + top_margin, cell[cell_row][cell_col].position.y + top_margin + open_bracket.rows), cv::Range(cell[cell_row][cell_col].position.x + gap, cell[cell_row][cell_col].position.x + gap + open_bracket.cols)));
    
    close_bracket.copyTo(sudoku_img(cv::Range(cell[cell_row][cell_col].position.y + top_margin, cell[cell_row][cell_col].position.y + top_margin + open_bracket.rows), cv::Range(close_bracket_x, close_bracket_x + open_bracket.cols)));
    
    
    /*Render the domain vlaues*/
    for(int i = 0; i < cell[cell_row][cell_col].domain.size(); i++)
    {
       
        int img_id = cell[cell_row][cell_col].domain[i] - 1;
        Mat domain_num_img;
        cv::resize(num_blue_img[img_id], domain_num_img, Size(num_width, num_height));
        int row_idx = i / 3;
        int col_idx = i % 3;
        int y = (row_idx + 1) * cell[cell_row][cell_col].cell_size.height * domain_top_margin + row_idx * num_height + cell[cell_row][cell_col].position.y + cell_top_pixel * cell[cell_row][cell_col].cell_size.height;
        int x = (col_idx + 1) * cell[cell_row][cell_col].cell_size.width * domain_left_margin + col_idx * num_width + cell[cell_row][cell_col].position.x + cell_left_pixel * cell[cell_row][cell_col].cell_size.width;
      
        if(x < sudoku_img.cols && x > 0 && y < sudoku_img.rows && y > 0 && (x + domain_num_img.cols) < sudoku_img.cols
           && (x + domain_num_img.cols) > 0 && (y + domain_num_img.rows) < sudoku_img.rows  )
        domain_num_img.copyTo(sudoku_img(cv::Range(y, y + domain_num_img.rows), cv::Range(x, x + domain_num_img.cols)));
        
        
    }
    
}



/*Rener the cell that has assignment*/
void VisualDisplay::renderCellValue(int cell_row, int cell_col)
{
    int img_idx = cell[cell_row][cell_col].assignment - 1;
    int margin_top = (cell[cell_row][cell_col].cell_size.height - num_img[img_idx].rows) / 2;
    int margin_left = (cell[cell_row][cell_col].cell_size.width - num_img[img_idx].cols) / 2;
    int start_row = cell[cell_row][cell_col].position.y + margin_top;
    int start_col = cell[cell_row][cell_col].position.x + margin_left;
    num_img[img_idx].copyTo(sudoku_img(cv::Range(start_row, start_row + num_blue_img[img_idx].rows), cv::Range(start_col, start_col + num_blue_img[img_idx].cols)));
}

/* Draw the buttons on the canvas */
void VisualDisplay::paintButtons()
{
    
    /*Render the buttons*/
    for(int i = 0; i < BUTTON_NUM; i++)
    {
        if(button[i].status == 0) //render ordinary image
        {
            {
            button[i].org_img.copyTo(sudoku_img(cv::Range(button[i].top_let.y, button[i].bot_rgh.y), cv::Range(button[i].top_let.x, button[i].bot_rgh.x)));
            }
        }
        else if(button[i].status == 1) //render the hover image
        {
            button[i].hov_img.copyTo(sudoku_img(cv::Range(button[i].top_let.y, button[i].bot_rgh.y), cv::Range(button[i].top_let.x, button[i].bot_rgh.x)));
        }
        else //render selected image
        {
            button[i].clk_img.copyTo(sudoku_img(cv::Range(button[i].top_let.y, button[i].bot_rgh.y), cv::Range(button[i].top_let.x, button[i].bot_rgh.x)));
        }
    }
    if(go_btn.status == 0 && go_btn.type ==1)
    {
        go_btn.org_img.copyTo(sudoku_img(cv::Range(go_btn.top_let.y, go_btn.bot_rgh.y), cv::Range(go_btn.top_let.x, go_btn.bot_rgh.x)));
    }
    else if(go_btn.status == 1 && go_btn.type ==1)
    {
        go_btn.hov_img.copyTo(sudoku_img(cv::Range(go_btn.top_let.y, go_btn.bot_rgh.y), cv::Range(go_btn.top_let.x, go_btn.bot_rgh.x)));
    }
    else if(go_btn.status == 2 && go_btn.type == 2)
    {
        go_btn.clk_img.copyTo(sudoku_img(cv::Range(go_btn.top_let.y, go_btn.bot_rgh.y), cv::Range(go_btn.top_let.x, go_btn.bot_rgh.x)));
    }
    else if(go_btn.status == 0 && go_btn.type == 2)
    {
        go_btn.org_img_2.copyTo(sudoku_img(cv::Range(go_btn.top_let.y, go_btn.bot_rgh.y), cv::Range(go_btn.top_let.x, go_btn.bot_rgh.x)));
    }
    else if(go_btn.status == 1 && go_btn.type == 2)
    {
        go_btn.hov_img_2.copyTo(sudoku_img(cv::Range(go_btn.top_let.y, go_btn.bot_rgh.y), cv::Range(go_btn.top_let.x, go_btn.bot_rgh.x)));
    }
    else if(go_btn.status == 2 && go_btn.type == 1)
    {
        go_btn.clk_img_2.copyTo(sudoku_img(cv::Range(go_btn.top_let.y, go_btn.bot_rgh.y), cv::Range(go_btn.top_let.x, go_btn.bot_rgh.x)));
    }
    
    /*Render the labels*/
    cv::putText(sudoku_img, "Back Track", Point(button[0].top_let.x + 40, button[0].top_let.y + 14), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(50, 50, 50), 1, CV_AA);
    cv::putText(sudoku_img, "Forward Check", Point(button[1].top_let.x + 40, button[1].top_let.y + 14), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(50, 50, 50), 1, CV_AA);
    cv::putText(sudoku_img, "Arc Consistency", Point(button[2].top_let.x + 40, button[2].top_let.y + 14), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(50, 50, 50), 1, CV_AA);
    cv::putText(sudoku_img, "Random Value", Point(button[3].top_let.x + 40, button[3].top_let.y + 14), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(50, 50, 50), 1, CV_AA);
    cv::putText(sudoku_img, "Show Domain", Point(button[4].top_let.x + 40, button[4].top_let.y + 14), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(50, 50, 50), 1, CV_AA);
    cv::putText(sudoku_img, "Ordering", Point(button[5].top_let.x + 40, button[5].top_let.y + 14), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(50, 50, 50), 1, CV_AA);
 

}

/*draw the number assignment and domain on the sudoku image.*/
void VisualDisplay::paintContainer()
{
    cv::Mat white_img(sudoku_blank.rows, sudoku_blank.cols + 300, CV_8UC3, cv::Scalar(255, 255, 255));
    white_img.copyTo(sudoku_img);
    
    sudoku_blank.copyTo(sudoku_img(cv::Range(0, sudoku_blank.rows), cv::Range(0, sudoku_blank.cols))); //copy the blank image
    
    /*Render Selected Cell*/
    if(chosen_cell != -1)
    {
        int row = chosen_cell / 9;
        int col = chosen_cell % 9;
        cv::rectangle(sudoku_img, cell[row][col].position, Point(cell[row][col].position.x + cell[row][col].cell_size.width, cell[row][col].position.y + cell[row][col].cell_size.height), cv::Scalar(255, 0, 0), 3);
    }
    
    /*Render the remaining cells*/
    for(int j = 0; j < 9; j++)
    {
        for(int i = 0; i < 9; i++)
        {
            /*If the cell has been assigned*/
            if(cell[j][i].assignment >= 1)
            {
                renderCellValue(j, i);
            }
            /*If the cell has not been assigned*/
            else{
                if(button[BUTTON_OPT_NUM + 1].status == 2)
                renderCellDomain(j, i);
            }
        }
    }
    
    /*Render the buttons and labels*/
    paintButtons();
    
    /*Render Goal Found*/
    if(goal_found == true)
        cv::putText(sudoku_img, "Goal is Found!!!", Point(sudoku_blank.cols * .4, sudoku_blank.rows * .4), FONT_HERSHEY_COMPLEX_SMALL, 2.2 , Scalar(0, 200, 0), 3, CV_AA);
    
}

/*Show the GUI: display the Sudoku image, display the number assignment, display the potential domain*/
void VisualDisplay::run()
{
    while(1)
    {
        
        /*In every interation, refresh the data from CSP first*/
        refreshData();
        
        
        paintContainer();
        
        imshow("Sudoku", sudoku_img);
        
        
        /*Control Button Refreshing*/
        for(int i = BUTTON_OPT_NUM + 3; i < BUTTON_NUM; i++)
        {
            if(button[i].status == 2)
            {
                waitKey(1);
                button[i].status = 1;
            }
        }
        
        /*For the Go Button*/
        if(go_btn.type == 2)
        {
            onGo();
        }
        if(goal_found)
        {
            go_btn.type = 1; //initially it is in ready to go state.
           // goal_found = 0; //initilly set goal is not found
        }
        
        /*Keyboard operation*/
        char c = waitKey(1);
        if(c == 27)
            break;
        else if(chosen_cell != -1)
        {
            int j = chosen_cell / 9;
            int i = chosen_cell % 9;
            switch (c) {
                case '1':
                    cell[j][i].assignment = 1;
                    csp_data.variables[j][i].assignement = 1; //update the corresponding values in CSP
                    csp_data.cur_state.values[j][i] = 1;
                    break;
                    
                case '2':
                    cell[j][i].assignment = 2;
                    csp_data.variables[j][i].assignement = 2; //update the corresponding values in CSP
                    csp_data.cur_state.values[j][i] = 2;
                    break;
                    
                    
                case '3':
                    cell[j][i].assignment = 3;
                    csp_data.variables[j][i].assignement = 3; //update the corresponding values in CSP
                    csp_data.cur_state.values[j][i] = 3;
                    break;
                    
                    
                case '4':
                    cell[j][i].assignment = 4;
                    csp_data.variables[j][i].assignement = 4; //update the corresponding values in CSP
                    csp_data.cur_state.values[j][i] = 4;
                    break;
                    
                    
                case '5':
                    cell[j][i].assignment = 5;
                    csp_data.variables[j][i].assignement = 5; //update the corresponding values in CSP
                    csp_data.cur_state.values[j][i] = 5;
                    break;
                    
                    
                case '6':
                    cell[j][i].assignment = 6;
                    csp_data.variables[j][i].assignement = 6; //update the corresponding values in CSP
                    csp_data.cur_state.values[j][i] = 6;
                    break;
                    
                    
                case '7':
                    cell[j][i].assignment = 7;
                    csp_data.variables[j][i].assignement = 7; //update the corresponding values in CSP
                    csp_data.cur_state.values[j][i] = 7;
                    break;
                    
                    
                case '8':
                    cell[j][i].assignment = 8;
                    csp_data.variables[j][i].assignement = 8; //update the corresponding values in CSP
                    csp_data.cur_state.values[j][i] = 8;
                    break;
                    
                    
                case '9':
                    cell[j][i].assignment = 9;
                    csp_data.variables[j][i].assignement = 9; //update the corresponding values in CSP
                    csp_data.cur_state.values[j][i] = 9;
                    break;
                    
                case '0':
                    csp_data.variables[j][i].assignement = 0; //update the corresponding values in CSP
                    csp_data.cur_state.values[j][i] = 0;
                    cell[j][i].assignment = 0;
                    
                    
                default:
                    break;
            }
            if(c >= '1' && c <= '9')
            {
                csp_data.assigned_variables.push(chosen_cell);
                goal_found = csp_data.goalCheck(csp_data.cur_state);
                if(goal_found)
                    printf("find goal\n");

                
                /*For forward checking use*/
                if(csp_data.alg_opt == 2)
                    csp_data.updateDomain(csp_data.cur_state);
                
                /*For arc consistency checking use*/
                if(csp_data.alg_opt == 3)
                    csp_data.arcConsistency(csp_data.cur_state);
            }
            
        }
        
    }
}