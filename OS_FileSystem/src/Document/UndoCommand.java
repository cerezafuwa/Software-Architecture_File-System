package Document;

/**
 * Created by hubeini on 2016/12/13.
 */
public class UndoCommand extends Command {
    UndoableTextArea text;

    public UndoCommand(UndoableTextArea text)
    {
        super("撤消");
        this.text = text;
    }

    @Override
    public void execute() {
        // TODO Auto-generated method stub
        text.undo();
    }
}
