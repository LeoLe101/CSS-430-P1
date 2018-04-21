import java.io.*;
import java.util.*;

class Shell extends Thread
{
  // default constructor
  public Shell()
  {
    SysLib.cout("Shell initiated!" + "\n");
    SysLib.cout("To terminate the Shell, type (q) or (exit)." + "\n");
  }

  // execution function for ';' symbol
  // this execution will have to wait
  // for this command to be done in order to continue
  public void sequentialExec(String semi_str)
  {
    // execute the command
    String[] cmd_line = SysLib.stringToArgs(semi_str); // split str to process
    if (cmd_line[cmd_line.length - 1].equals(";"))
      Arrays.copyOf(cmd_line, cmd_line.length-1); // remove the last char ';'
    int tid = SysLib.exec(cmd_line);
    if (tid < 0)
      SysLib.cerr("Invalid Command!" + "\n");
    else
    {
      int join_id = SysLib.join();
      if (join_id < 0)
        SysLib.cerr("Unable to join!" + "\n");
      while(join_id != tid)
      {
        join_id = SysLib.join();
        if (join_id < 0)
          SysLib.cerr("Unable to join!" + "\n");
      }
    }
  }

  // execution function for '&' symbol
  // this execution will not wait for
  // any command to be done in order to execute
  public void concurrentExec(String amp_str)
  {
    // execute the command
    String[] cmd_line = SysLib.stringToArgs(amp_str); // split str to process
    if (cmd_line[cmd_line.length - 1].equals("&"))
      Arrays.copyOf(cmd_line, cmd_line.length-1); // remove the last char '&'
    int tid = SysLib.exec(cmd_line);
    if (tid < 0)
      SysLib.cerr("Invalid Command!" + "\n");
  }

  // command handler that will split the ';' and '&'
  // from each argument and execute them properly
  public boolean cmdHandler(StringBuffer s)
  {
    // stop processing the command when user wants to exit
    if ((s.toString().equals("exit")) || (s.toString().equals("q")))
      return true;
    // case no argument
    if (s.toString().length() < 1)
      return false;
    // process command
    // contain both ';' and '&' in this argument
    if (s.toString().contains("&") && s.toString().contains(";"))
    {
      // use regrex to split the string
      String[] cmd = s.toString().split("((?<=;)|(?<=&))");
      for (int i = 0; i < cmd.length; ++i)
      {
        String[] str = SysLib.stringToArgs(cmd[i]);
        // case if the end of string is ';'
        if (str[str.length - 1].equals(";"))
          sequentialExec(cmd[i]);
        // case if the end of string is '&'
        else if (str[str.length - 1].equals("&"))
          concurrentExec(cmd[i]);
        // case if the end of string is eof
        else
          sequentialExec(cmd[i]);
      }
    }
    // contain only ';' in the argument
    else if (s.toString().contains(";"))
    {
      String[] semi_string = s.toString().split(";");
      for (int i = 0; i < semi_string.length; ++i)
        sequentialExec(semi_string[i]);
    }
    // contain only '&' in the argument
    else if (s.toString().contains("&"))
    {
      String[] amp_string = s.toString().split("&");
      for (int i = 0; i < amp_string.length; ++i)
        concurrentExec(amp_string[i]);
    }
    // case only 1 argument
    else
      sequentialExec(s.toString());
    return false;
  }

  // main function or run function
  public void run()
  {
    int count = 1;
    boolean terminate = false;
    while (terminate == false)
    {
      SysLib.cout("Shell[" + count + "]% ");
      // get command from the buffer
      StringBuffer buffer = new StringBuffer();
      SysLib.cin(buffer);
      // check for shell termination
      if (cmdHandler(buffer) == true)
        terminate = true;
      ++count; // increment shell count
    }
    SysLib.sync(); // write back all on-memory data into disk
    SysLib.exit(); // exit after done
  }
}