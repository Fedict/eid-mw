package be.fedict.eidviewer.lib;

/**
 *
 * @author frank
 */
public class EidException extends RuntimeException
{
    public EidException()
    {
        super();
    }

    public EidException(String message)
    {
        super(message);
    }

    public EidException(String message, Throwable cause)
    {
        super(message,cause);
    }

    public EidException(Throwable cause)
    {
        super(cause);
    }
}
