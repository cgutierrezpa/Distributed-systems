
package localhost._8080.md5;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the localhost._8080.md5 package. 
 * <p>An ObjectFactory allows you to programatically 
 * construct new instances of the Java representation 
 * for XML content. The Java representation of XML 
 * content can consist of schema derived interfaces 
 * and classes representing the binding of schema 
 * type definitions, element declarations and model 
 * groups.  Factory methods for each of these are 
 * provided in this class.
 * 
 */
@XmlRegistry
public class ObjectFactory {

    private final static QName _MD5_QNAME = new QName("http://localhost:8080/MD5", "MD5");
    private final static QName _MD5Response_QNAME = new QName("http://localhost:8080/MD5", "MD5Response");
    private final static QName _Exception_QNAME = new QName("http://localhost:8080/MD5", "Exception");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: localhost._8080.md5
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link Exception }
     * 
     */
    public Exception createException() {
        return new Exception();
    }

    /**
     * Create an instance of {@link MD5 }
     * 
     */
    public MD5 createMD5() {
        return new MD5();
    }

    /**
     * Create an instance of {@link MD5Response }
     * 
     */
    public MD5Response createMD5Response() {
        return new MD5Response();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link MD5 }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://localhost:8080/MD5", name = "MD5")
    public JAXBElement<MD5> createMD5(MD5 value) {
        return new JAXBElement<MD5>(_MD5_QNAME, MD5 .class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link MD5Response }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://localhost:8080/MD5", name = "MD5Response")
    public JAXBElement<MD5Response> createMD5Response(MD5Response value) {
        return new JAXBElement<MD5Response>(_MD5Response_QNAME, MD5Response.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link Exception }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://localhost:8080/MD5", name = "Exception")
    public JAXBElement<Exception> createException(Exception value) {
        return new JAXBElement<Exception>(_Exception_QNAME, Exception.class, null, value);
    }

}
