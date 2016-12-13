package Document;

/**
 * Created by hubeini on 2016/12/13.
 */
public class RedoCommand extends Command {

    UndoableTextArea text;

    public RedoCommand(UndoableTextArea text)
    {
        super("重做");
        this.text = text;
    }

    @Override
    public void execute() {
        // TODO Auto-generated method stub
        text.redo();
    }

}

