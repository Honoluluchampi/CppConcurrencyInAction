#include <thread>
#include <string>

// pseudo code for document editor app.
void edit_document(std::string const& filename)
{
    open_document_and_display_gui(filename);
    while(!done_editting()){
        user_command cmd=get_user_input();
        // launching new thread for new window
        if(cmd.type==open_new_window){
            std::string const new_name=get_filename_from_user();
            std::thread t(edit_document, new_name);
            t.detach();
        }
        else{
            process_user_input(cmd);
        }
    }
}