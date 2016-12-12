package Document;

public interface I_DocumentFactory{
	
	void setFatherAddress(String str);
	String getSystemTime();
	void setCreateTime();
	void setVisitTime();
	void setModifiTime();
	void showProperty();
	void getProperty(Block block);
}


