
#include <stdio.h>
#include "CSP.h"


bool CSP::goalCheck(const State state)
{
	/*For every cell, check the consistency*/
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			/*Check each row for consistency*/
			for (int a = 0; a < 9; a++)
			{
				if ((state.values[i][j] == state.values[i][a]) && (a != j))
					return false;
			}
			/*Check each column for consistency*/
			for (int a = 0; a < 9; a++)
			{
				if ((state.values[i][j] == state.values[a][j]) && (a != i))
					return false;
			}
			/*Check each block for consistency*/
			for (int a = (i / 3) * 3; a < (i / 3) * 3 + 3; a++)
			{
				for (int b = (j / 3) * 3; b < (j / 3) * 3 + 3; b++)
				{
					if ((state.values[i][j] == state.values[a][b]) && (a != i)&&(b!=j))
						return false;
				}
			}
		}
	}
    return true;
}
/*Update Domain for the forward checking*/
void CSP::updateDomain(const State state)
{
	int value;
	/*For each cell which assignment equals to '0', reset the domain (1,2,3,4,5,6,7,8,9)*/
	for (int j = 0; j < 81; j++)
	{
		if ((variables[j / 9][j % 9].assignement == 0) && (variables[j/9][j%9].domain.size()!=9))
		{
			/*clear the domain and reset to make sure no repeating number*/
			variables[j / 9][j % 9].domain.clear();
			for (int i = 1; i < 10; i++)
			{
				/*assign '1,2,3,4,5,6,7,8,9' to the domain*/
				variables[j / 9][j % 9].domain.push_back(i);
			}
		}	
	}
	/*check every cell to update the domain based on conditions*/
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9;j++)
		{		
			if ((state.values[i][j] != 0) && (variables[i][j].assignement!=0))
			{
				/*first condition: the same row which the assigned cell is in */
				value = state.values[i][j];
				for (int a = 0; a < 9; a++)
				{
					if (a != j)
					{
						/*update the domain by delete the assigned value for each cell in the same row */
						variables[i][a].domain.erase(remove(variables[i][a].domain.begin(), variables[i][a].domain.end(), value), variables[i][a].domain.end());
					}
				}
				/*second condition: the same column which the assigned cell is in */
				for (int a = 0; a < 9; a++)
				{
					if (a != i)
					{
						/*update the domain by delete the assigned value for each cell in the same column */
						variables[a][j].domain.erase(remove(variables[a][j].domain.begin(), variables[a][j].domain.end(), value), variables[a][j].domain.end());
					}
				}
				/*third condition: the same block which the assigned cell is in */
				for (int a = (i / 3) * 3; a < (i / 3) * 3 + 3; a++)
				{
					for (int b = (j / 3) * 3; b < (j / 3) * 3 + 3; b++)
					{
						if ((a != i) && (b != j))
						{
							/*update the domain by delete the assigned value for each cell in the same block */
							variables[a][b].domain.erase(remove(variables[a][b].domain.begin(), variables[a][b].domain.end(), value), variables[a][b].domain.end());
						}
					}
				}
			}
		}
	}
}
/*Arc consistency use*/
void CSP::arcConsistency(const State state)
{
	int value;
	/*for each unassigned value, reset the domain to become '1,2,3,4,5,6,7,8,9'*/
	for (int j = 0; j < 81; j++)
	{
		if ((variables[j / 9][j % 9].assignement == 0) && (variables[j / 9][j % 9].domain.size() != 9))
		{
			variables[j / 9][j % 9].domain.clear();
			for (int i = 1; i < 10; i++)
			{
				variables[j / 9][j % 9].domain.push_back(i);
			}
		}
	}
	/*create the Q to store all the arcs of assigned cell*/
	queue<Arrow> Q;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			/*if the cell has been assigned, just remove all the value in the domian but only left the assigned value*/
			if ((state.values[i][j] != 0)&&(variables[i][j].assignement!=0))
			{
				value = state.values[i][j];
				if (variables[i][j].domain.size() != 1)
				{
					variables[i][j].domain.clear();
					variables[i][j].domain.push_back(value);
				}
				/*for each assigned cell, creat an arrow from the cell which is in the same row of assigned cell to the assigned cell*/
				for (int a = 0; a < 9; a++)
				{
					if (a != j)
					{
						Arrow arrow(i * 9 + j, i * 9 + a);
						Q.push(arrow);
						Arrow arrow1(i * 9 + a,i * 9 + j );
						Q.push(arrow1);
					}
				}
				/*for each assigned cell, creat an arrow from the cell which is in the same column of assigned cell to the assigned cell*/
				for (int a = 0; a < 9; a++)
				{
					if (a != i)
					{
						Arrow arrow(i * 9 + j, a * 9 + j);
						Q.push(arrow);
						Arrow arrow1(a * 9 + j, i * 9 + j);
						Q.push(arrow1);
					}
				}
				/*for each assigned cell, creat an arrow from the cell which is in the same block of assigned cell to the assigned cell*/
				for (int a = (i / 3) * 3; a < (i / 3) * 3 + 3; a++)
				{
					for (int b = (j / 3) * 3; b < (j / 3) * 3 + 3; b++)
					{
						if ( (a != i) && (b != j))
						{
							Arrow arrow(i * 9 + j, a * 9 + b);
							Q.push(arrow);
							Arrow arrow1(a * 9 + b, i * 9 + j);
							Q.push(arrow1);
						}
					}
				}
			}
		}
	}
	/*the code is the same as the teacher taught in class*/
	while (!Q.empty())
	{
		/*record the updated cell*/
		delete_sign = 0;
		Arrow cur = Q.front();
		Q.pop();
		int value;
		/*check whether the tail has deleted value*/
		int size1,size2;
		/*if the head has only one value, the tail must delete the value if the tail has the same one*/
		if (variables[cur.head / 9][cur.head % 9].domain.size() == 1)
		{
			size1 = variables[cur.tail / 9][cur.tail % 9].domain.size();
			value = variables[cur.head / 9][cur.head % 9].domain[0];
			variables[cur.tail / 9][cur.tail % 9].domain.erase(remove(variables[cur.tail / 9][cur.tail % 9].domain.begin(), variables[cur.tail / 9][cur.tail % 9].domain.end(), value), variables[cur.tail / 9][cur.tail % 9].domain.end());
			size2 = variables[cur.tail / 9][cur.tail % 9].domain.size();
			/*the tail deletes value*/
			if(size1 != size2)
				delete_sign = 1;
		}
		else if (variables[cur.head / 9][cur.head % 9].domain.size() == 0)
		{
			size1 = variables[cur.tail / 9][cur.tail % 9].domain.size();
			
			variables[cur.tail / 9][cur.tail % 9].domain.clear();
			size2 = variables[cur.tail / 9][cur.tail % 9].domain.size();
			/*the tail deletes value*/
			if (size1 != size2)
				delete_sign = 1;
		}
		/*if the tail deletes value*/
		/*add all the neighbor cells of tail into the Q to check arc consistency*/
		if (delete_sign == 1)
		{
			/*add the same row*/
			for (int a = 0; a < 9; a++)
			{
				if (a != cur.tail%9)
				{
					Arrow arrow(cur.tail, (cur.tail/9)*9+a);
					Q.push(arrow);
					Arrow arrow1((cur.tail / 9) * 9 + a,cur.tail );
					Q.push(arrow1);
				}
			}
			/*add the same column*/
			for (int a = 0; a < 9; a++)
			{
				if (a != cur.tail / 9)
				{
					Arrow arrow(cur.tail, a * 9 + cur.tail % 9);
					Q.push(arrow);
					Arrow arrow1(a * 9 + cur.tail % 9,cur.tail );
					Q.push(arrow1);
				}
			}
			/*add the same block*/
			for (int a = (cur.tail / 9 / 3) * 3; a < (cur.tail / 9 / 3) * 3 + 3; a++)
			{
				for (int b = (cur.tail % 9 / 3) * 3; b < (cur.tail % 9 / 3) * 3 + 3; b++)
				{
					if ((a != cur.tail / 9) && (b != cur.tail % 9))
					{
						Arrow arrow(cur.tail, a*9+b);
						Q.push(arrow);
						Arrow arrow1(a * 9 + b,cur.tail );
						Q.push(arrow1);
					}
				}
			}	
		}
	}
}




CSP::CSP()
{
    /*Initially assign the domain, assignment for each variable and initialize the current state*/
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            variables[y][x].assignement = 0; //Initialize the assignment
            
            /*Initialize the domain*/
            for(int i = 1; i <= 9; i++)
            {
                variables[y][x].domain.push_back(i);
            }
            
            cur_state.values[y][x] = 0; //Initizlize the current state
            
        }
    }
    
    alg_opt = 1; //initially set it as back track
    
    srand(time(NULL));
    random = 0;
}


CSP::~CSP()
{
    
}




void CSP::setData(int *data)
{
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            int idx = y * 9 + x;
            variables[y][x].assignement = data[idx]; //Initialize the assignment
            cur_state.values[y][x] = data[idx]; //Initizlize the current state
            
        }
    }
}

void CSP::clearData()
{
    /*Initially assign the domain, assignment for each variable and initialize the current state*/
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            variables[y][x].assignement = 0; //Initialize the assignment
            
            /*Initialize the domain*/
            variables[y][x].domain.clear();
            for(int i = 1; i <= 9; i++)
            {
                variables[y][x].domain.push_back(i);
            }
            
            cur_state.values[y][x] = 0; //Initizlize the current state
            
        }
    }
    
    /*Check whether a random domain is use*/
    if(random == 1)
        reshuffleDomain();
    
    repeating_list.clear();
    while(!assigned_variables.empty())
    {
        assigned_variables.pop();
        repeating_list.clear();
    }
    
}


void CSP::reshuffleDomain()
{
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        std::random_shuffle( variables[y][x].domain.begin(), variables[y][x].domain.end() );
    }
}

void CSP::sortDomain()
{
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        std::sort( variables[y][x].domain.begin(), variables[y][x].domain.end() );
    }
}

/*Cancel last assignment*/
int CSP::goBack(int *chosen_cell)
{
    if(assigned_variables.size() > 0)
    {
        int cur_id = assigned_variables.top(); /*Remove last options*/
        assigned_variables.pop(); //pop out last option
        int y = cur_id / 9;
        int x = cur_id % 9;
        
        variables[y][x].assignement = 0; //assign the cell to zero
        cur_state.values[y][x] = 0; //update the assignment
        *chosen_cell = cur_id;
        
       // printf("(%d, %d)\n", y, x);
        if(alg_opt == 2)
        {
            updateDomain(cur_state);
        }
        else if (alg_opt == 3)
        {
            arcConsistency(cur_state);
        }
        
    }
    
    return goalCheck(cur_state);
    
}


bool CSP::arcCheckingOrder(int *chosen_cell)
{
    arcConsistency(cur_state);
    
    
    
    /*First go through all the variables and do backtrack if there is no empty domain */
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() == 0)
        {
            int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
            while (available_assignemnt == 0) {
                int cur_id = assigned_variables.top();
                int y = cur_id / 9;
                int x = cur_id % 9;
                variables[y][x].assignement = 0;
                cur_state.values[y][x] = 0;
                arcConsistency(cur_state);
                
                
                for(int i = 0; i < variables[y][x].domain.size(); i++)
                {
                    State temp_state;
                    temp_state = cur_state;
                    temp_state.values[y][x] = variables[y][x].domain[i];
                    if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
                    {
                        cur_state = temp_state;
                        variables[y][x].assignement = variables[y][x].domain[i];
                        repeating_list.push_back(temp_state);
                        available_assignemnt = 1;
                        *chosen_cell = cur_id;
                        arcConsistency(cur_state);
                        return false; //get out of the current varaible assignment
                    }
                }
                
                if(available_assignemnt == 0) //if all the domain values have been tried for current variable
                {
                    variables[y][x].assignement = 0;
                    cur_state.values[y][x] = 0;
                    assigned_variables.pop();
                    
                }
            }
            
        }
        
    }
    
    /*If there is no variable that has empty domain, then assign variable here*/
    /*First go through all the variables and do backtrack if there is no empty domain */
    int count = 0;
    while (count < 81)
    {
        /*Find the index of minimum number of domain*/
        int min_idx = 0;
        int min_num = 10; //because the maximum number of domain is 10
        for(int i = 0; i < 81; i++)
        {
            int y = i / 9;
            int x = i % 9;
            if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
            {
                if (variables[y][x].domain.size() < min_num) {
                    min_idx = i;
                    min_num = variables[y][x].domain.size();
                }
            }
        }
        
        int y = min_idx / 9;
        int x = min_idx % 9;
        
        /*If there is any varable has not been assigned yet, assign it and return it*/
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
        {
            /*Find the smalles number in domain to assign it. Here no update domain for bracktrack*/
            int id_min = 0;
            cur_state.values[y][x] = variables[y][x].domain[id_min];
            variables[y][x].assignement = variables[y][x].domain[id_min];
            assigned_variables.push(min_idx); //push the variable into stack, which will be used for backtrack (or DFS)
            repeating_list.push_back(cur_state); //make this state into the repeat_list
            *chosen_cell = 9 * y + x;
            
            arcConsistency(cur_state); //Every time modify the assignment update the domain
            
            return false;
        }
        
        count++;
        
    }
    
    if(goalCheck(cur_state))
    {
        printf("find the goal\n");
        return true;
    }
	else
	{
		int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
		while (available_assignemnt == 0) {
			int cur_id = assigned_variables.top();
			int y = cur_id / 9;
			int x = cur_id % 9;
			variables[y][x].assignement = 0;
			cur_state.values[y][x] = 0;
			arcConsistency(cur_state);
			for(int i = 0; i < variables[y][x].domain.size(); i++)
			{
				State temp_state;
				temp_state = cur_state;
				temp_state.values[y][x] = variables[y][x].domain[i];
				if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
				{
					cur_state = temp_state;
					variables[y][x].assignement = variables[y][x].domain[i];
					repeating_list.push_back(cur_state);
					available_assignemnt = 1;
					*chosen_cell = cur_id;
					break; //get out of the current varaible assignment
				}
			}

			if(available_assignemnt == 0) //if all the domain values have been tried for current variable
			{

				assigned_variables.pop();

			}
		}

		return false;
	}
    return false;
    
}

/*arcChecking without ordering*/
bool CSP::arcChecking(int *chosen_cell)
{
    arcConsistency(cur_state);
    /*First go through all the variables and do backtrack if there is no empty domain */
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() == 0)
        {
            int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
			while (available_assignemnt == 0) {
				
					int cur_id = assigned_variables.top();
					int y = cur_id / 9;
					int x = cur_id % 9;
					variables[y][x].assignement = 0;

					cur_state.values[y][x] = 0;
					arcConsistency(cur_state);
			

					for (int i = 0; i < variables[y][x].domain.size(); i++)
					{
						State temp_state;
						temp_state = cur_state;
						temp_state.values[y][x] = variables[y][x].domain[i];
						if (std::find(repeating_list.begin(), repeating_list.end(), temp_state) == repeating_list.end()) //if not in the repeating list
						{
							cur_state = temp_state;
							variables[y][x].assignement = variables[y][x].domain[i];
							repeating_list.push_back(temp_state);
							available_assignemnt = 1;
							*chosen_cell = cur_id;
							arcConsistency(cur_state);
							return false; //get out of the current varaible assignment
						}
					}

					if (available_assignemnt == 0) //if all the domain values have been tried for current variable
					{
						variables[y][x].assignement = 0;
						cur_state.values[y][x] = 0;
						assigned_variables.pop();

					}
				
            }
            
        }
    }
    
    /*If there is no variable that has empty domain, then assign variable here*/
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        /*If there is any varable has not been assigned yet, assign it and return it*/
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
        {
            /*Find the smalles number in domain to assign it. Here no update domain for bracktrack*/
            int id_min = 0;
            cur_state.values[y][x] = variables[y][x].domain[id_min];
            variables[y][x].assignement = variables[y][x].domain[id_min];
            assigned_variables.push(i); //push the variable into stack, which will be used for backtrack (or DFS)
            repeating_list.push_back(cur_state); //make this state into the repeat_list
            *chosen_cell = 9 * y + x;
            
            arcConsistency(cur_state); //Every time modify the assignment update the domain

            return false;
        }
        
    }
    
    if(goalCheck(cur_state))
    {
        printf("find the goal\n");
        return true;
    }
	else
	{
		int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
		while (available_assignemnt == 0) {
			int cur_id = assigned_variables.top();
			int y = cur_id / 9;
			int x = cur_id % 9;
			variables[y][x].assignement = 0;
			cur_state.values[y][x] = 0;
			arcConsistency(cur_state);
			for(int i = 0; i < variables[y][x].domain.size(); i++)
			{
				State temp_state;
				temp_state = cur_state;
				temp_state.values[y][x] = variables[y][x].domain[i];
				if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
				{
					cur_state = temp_state;
					variables[y][x].assignement = variables[y][x].domain[i];
					repeating_list.push_back(cur_state);
					available_assignemnt = 1;
					*chosen_cell = cur_id;
					break; //get out of the current varaible assignment
				}
			}

			if(available_assignemnt == 0) //if all the domain values have been tried for current variable
			{

				assigned_variables.pop();

			}
		}

		return false;
	}
    return false;

}



/*Forward Checking algorithm*/
bool CSP::forwardChecking(int *chosen_cell)
{
    updateDomain(cur_state); //the first step is based on current setting to update the domain
    
    
    
    /*First go through all the variables and do backtrack whether there is an empty domain */
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() == 0)
        {
            int available_assignemnt = 0; //an indicatior whether there are possible possible varaibles to be re-assigned
            while (available_assignemnt == 0) {
                int cur_id = assigned_variables.top();
                int y = cur_id / 9;
                int x = cur_id % 9;
                variables[y][x].assignement = 0;
                cur_state.values[y][x] = 0;
                updateDomain(cur_state);
                for(int i = 0; i < variables[y][x].domain.size(); i++)
                {
                    State temp_state;
                    temp_state = cur_state;
                    temp_state.values[y][x] = variables[y][x].domain[i];
                    if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
                    {
                        cur_state = temp_state;
                        variables[y][x].assignement = variables[y][x].domain[i];
                        repeating_list.push_back(temp_state);
                        available_assignemnt = 1;
                        *chosen_cell = cur_id;
                        updateDomain(cur_state);
                        return false; //get out of the current varaible assignment
                    }
                }
                
                if(available_assignemnt == 0) //if all the domain values have been tried for current variable
                {
                    variables[y][x].assignement = 0;
                    cur_state.values[y][x] = 0;
                    assigned_variables.pop();
                    
                }
            }
            
        }
    }
    
    /*If there is no variable that has empty domain, then assign variable here*/
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        /*If there is any varable has not been assigned yet, assign it and return it*/
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
        {
            /*Find the smalles number in domain to assign it. Here no update domain for bracktrack*/
            int id_min = 0;
            cur_state.values[y][x] = variables[y][x].domain[id_min];
            variables[y][x].assignement = variables[y][x].domain[id_min];
            assigned_variables.push(i); //push the variable into stack, which will be used for backtrack (or DFS)
            repeating_list.push_back(cur_state); //make this state into the repeat_list
            *chosen_cell = 9 * y + x;
            
            updateDomain(cur_state); //Every time modify the assignment update the domain
            
            return false;
        }

    }
    
    if(goalCheck(cur_state))
    {
        printf("find the goal\n");
        return true;
    }else
	{
		int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
		while (available_assignemnt == 0) {
			int cur_id = assigned_variables.top();
			int y = cur_id / 9;
			int x = cur_id % 9;
			variables[y][x].assignement = 0;
			cur_state.values[y][x] = 0;
			updateDomain(cur_state);
			for(int i = 0; i < variables[y][x].domain.size(); i++)
			{
				State temp_state;
				temp_state = cur_state;
				temp_state.values[y][x] = variables[y][x].domain[i];
				if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
				{
					cur_state = temp_state;
					variables[y][x].assignement = variables[y][x].domain[i];
					repeating_list.push_back(cur_state);
					available_assignemnt = 1;
					*chosen_cell = cur_id;
					break; //get out of the current varaible assignment
				}
			}

			if(available_assignemnt == 0) //if all the domain values have been tried for current variable
			{

				assigned_variables.pop();

			}
		}

		return false;
	}
    return false;
    
}


/*Forward Checking algorithm*/
bool CSP::forwardCheckingOrder(int *chosen_cell)
{

    updateDomain(cur_state); //the first step is based on current setting to update the domain
    
    
    
    /*First go through all the variables and do backtrack whether there is an empty domain */
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() == 0)
        {
            int available_assignemnt = 0; //an indicatior whether there are possible possible varaibles to be re-assigned
            while (available_assignemnt == 0) {
                int cur_id = assigned_variables.top();
                int y = cur_id / 9;
                int x = cur_id % 9;
                variables[y][x].assignement = 0;
                cur_state.values[y][x] = 0;
                updateDomain(cur_state);
                for(int i = 0; i < variables[y][x].domain.size(); i++)
                {
                    State temp_state;
                    temp_state = cur_state;
                    temp_state.values[y][x] = variables[y][x].domain[i];
                    if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
                    {
                        cur_state = temp_state;
                        variables[y][x].assignement = variables[y][x].domain[i];
                        repeating_list.push_back(temp_state);
                        available_assignemnt = 1;
                        *chosen_cell = cur_id;
                        updateDomain(cur_state);
                        return false; //get out of the current varaible assignment
                    }
                }
                
                if(available_assignemnt == 0) //if all the domain values have been tried for current variable
                {
                    variables[y][x].assignement = 0;
                    cur_state.values[y][x] = 0;
                    assigned_variables.pop();
                    
                }
            }
            
        }
    }
    
    
    int count = 0;
    while (count < 81)
    {
        /*Find the index of minimum number of domain*/
        int min_idx = 0;
        int min_num = 10; //because the maximum number of domain is 10
        for(int i = 0; i < 81; i++)
        {
            int y = i / 9;
            int x = i % 9;
            if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
            {
                if (variables[y][x].domain.size() < min_num) {
                    min_idx = i;
                    min_num = variables[y][x].domain.size();
                }
            }
        }
        
        int y = min_idx / 9;
        int x = min_idx % 9;
        
        /*If there is any varable has not been assigned yet, assign it and return it*/
        if(cur_state.values[y][x] == 0 && variables[y][x].domain.size() > 0)
        {
            /*Find the smalles number in domain to assign it. Here no update domain for bracktrack*/
            int id_min = 0;
            cur_state.values[y][x] = variables[y][x].domain[id_min];
            variables[y][x].assignement = variables[y][x].domain[id_min];
            assigned_variables.push(min_idx); //push the variable into stack, which will be used for backtrack (or DFS)
            repeating_list.push_back(cur_state); //make this state into the repeat_list
            *chosen_cell = 9 * y + x;
            
            updateDomain(cur_state); //Every time modify the assignment update the domain
            
            return false;
        }
        
        count++;
    }
    
    if(goalCheck(cur_state))
    {
        printf("find the goal\n");
        return true;
    }
	else
	{
		int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
		while (available_assignemnt == 0) {
			int cur_id = assigned_variables.top();
			int y = cur_id / 9;
			int x = cur_id % 9;
			variables[y][x].assignement = 0;
			cur_state.values[y][x] = 0;
			updateDomain(cur_state);
			for(int i = 0; i < variables[y][x].domain.size(); i++)
			{
				State temp_state;
				temp_state = cur_state;
				temp_state.values[y][x] = variables[y][x].domain[i];
				if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
				{
					cur_state = temp_state;
					variables[y][x].assignement = variables[y][x].domain[i];
					repeating_list.push_back(cur_state);
					available_assignemnt = 1;
					*chosen_cell = cur_id;
					break; //get out of the current varaible assignment
				}
			}

			if(available_assignemnt == 0) //if all the domain values have been tried for current variable
			{

				assigned_variables.pop();

			}
		}

		return false;
	}

    return false;
    
}



/*Back Track to solve the proble*/
bool CSP::backTrack(int *chosen_cell)
{
    
  
    
    for(int i = 0; i < 81; i++)
    {
        int y = i / 9;
        int x = i % 9;
        
        
        /*If there is any varable has not been assigned yet, assign it and break*/
        if(cur_state.values[y][x] == 0)
        {
            
            /*Find the smalles number in domain to assign it. Here no update domain for bracktrack*/
            int id_min = 0;
            cur_state.values[y][x] = variables[y][x].domain[id_min];
            variables[y][x].assignement = variables[y][x].domain[id_min];
            assigned_variables.push(i); //push the variable into stack, which will be used for backtrack (or DFS)
            repeating_list.push_back(cur_state); //make this state into the repeat_list
            *chosen_cell = 9 * y + x;
            return false;
            
        }
    }
    
    /*If all the the variable are assigned*/
    {
        if(assigned_variables.size() == 0)//reset all the variables if there are no any varaibles assigned yet
        {
            for(int i = 0; i < 81; i++)
            {
                assigned_variables.push(i);
            }
        }
        
        if(goalCheck(cur_state))
        {
            printf("find the goal\n");
            return true;
        }
        else
        {
            int available_assignemnt = 0; //an indicatior whether there are possible varaibles to be re-assigned
            while (available_assignemnt == 0) {
                int cur_id = assigned_variables.top();
                int y = cur_id / 9;
                int x = cur_id % 9;
                
                
                for(int i = 0; i < variables[y][x].domain.size(); i++)
                {
                    State temp_state;
                    temp_state = cur_state;
                    temp_state.values[y][x] = variables[y][x].domain[i];
                    if (std::find(repeating_list.begin(), repeating_list.end(), temp_state)==repeating_list.end()) //if not in the repeating list
                    {
                        cur_state = temp_state;
                        variables[y][x].assignement = variables[y][x].domain[i];
                        repeating_list.push_back(cur_state);
                        available_assignemnt = 1;
                        *chosen_cell = cur_id;
                        break; //get out of the current varaible assignment
                    }
                }
                
                if(available_assignemnt == 0) //if all the domain values have been tried for current variable
                {
                    variables[y][x].assignement = 0;
                    cur_state.values[y][x] = 0;
                    assigned_variables.pop();
                    
                }
            }
            
            return false;
        }
    }
}