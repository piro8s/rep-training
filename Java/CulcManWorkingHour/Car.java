class Car
{
	private int num;
	private double gas;
	
	public Car()
	{
		num=0;
		gas=0.0;
		System.out.println("車を作成しました。");
	}
	
	public void setCar(int n, double g)
	{
		num = n;
		gas = g;
		
		System.out.println("車のナンバーを" + num + "、ガソリン量を" + gas + "に設定しました。");
	}
	
	public void show()
	{
		System.out.println("車のナンバーは" + num + "です。");
		System.out.println("ガソリン量は" + gas + "です。");
	}
}