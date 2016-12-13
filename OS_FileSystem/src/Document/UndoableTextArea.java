package Document;

import java.awt.TextArea;
import java.awt.event.*;
import java.util.Hashtable;
import javax.swing.undo.*;


import javax.swing.undo.StateEditable;

public class UndoableTextArea extends TextArea implements StateEditable {

    private UndoManager undoManager;
    private final static String KEY_STATE="UndoableTextAreaKey";
    private StateEdit currentEdit;
    boolean textChanged = false;

    public UndoableTextArea()
    {
        super();
        initUndoable();
    }
    public UndoableTextArea(String string)
    {
        super(string);
        initUndoable();
    }

    //存储状态
    public void storeState(Hashtable<Object, Object> hashTable) {
        hashTable.put(KEY_STATE, this.getText());
    }

    //还原状态
    public void restoreState(Hashtable<?, ?> hashTable) {
        Object data = hashTable.get(KEY_STATE);
        if(data!=null)
        {
            this.setText((String)data);
        }
    }


    //撤销方法
    public boolean undo()
    {
        try
        {
            undoManager.undo();
            System.out.println("undo=" + undoManager.toString());
            return true;
        }catch(CannotUndoException e)
        {

        }
        return false;
    }

    //重做方法
    public boolean redo()
    {
        try
        {
            undoManager.redo();
            System.out.println("redo=" + undoManager.toString());
            return true;
        }catch(CannotRedoException e)
        {

        }
        return false;
    }

    private void initUndoable()
    {
        this.undoManager = new UndoManager();

        this.currentEdit = new StateEdit(this);

        this.addKeyListener(new KeyAdapter()
        {
            public void keyReleased(KeyEvent event)
            {
                if(event.isActionKey())
                {

                }else
                {
                    textChanged = true;
                    takeSnapshot();
                }
            }
        });

        this.addFocusListener(new FocusAdapter()
        {
            public void focusLost(FocusEvent event)
            {

            }
        });

    }

    private void takeSnapshot()
    {
        if(textChanged)
        {
            this.currentEdit.end();

            this.undoManager.addEdit(this.currentEdit);

            this.currentEdit = new StateEdit(this);

            System.out.println("takeSnapshot=" + undoManager.toString());

            textChanged = false;
        }
    }

}
