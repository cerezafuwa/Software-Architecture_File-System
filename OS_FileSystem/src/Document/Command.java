package Document;

import javax.swing.*;
import java.awt.*;

/**
 * Created by hubeini on 2016/12/13.
 */
public abstract class Command extends JMenuItem{
    public Command(String caption)
    {
        super(caption);
    }

    public abstract void execute();
}
