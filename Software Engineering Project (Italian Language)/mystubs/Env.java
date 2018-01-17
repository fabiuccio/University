package mystubs;

import hcs.model.Clock;

import java.util.Observable;
import java.util.Observer;
import java.util.Random;

import stubs.IPhysicalEnvironment;

/**
 * 
 * @author vincenzo.delia@polito.it
 *
 */
public class Env implements IPhysicalEnvironment, Observer{

	private double temp;
	private boolean rain;
	private long seconds;
	private Random r;
	
	public Env() {
		Clock.getInstance().addObserver(this);
		temp = 20;
		rain = false;
		seconds = 0;
		r = new Random();
	}
	
	public double getTemperature() {
		return temp;
	}

	public boolean isRainingSwitchClosed() {
		return rain;
	}

	public void update(Observable o, Object arg) {
		// Rain cambia con probabilità 20%
		int next = r.nextInt(10)+1;
		if (next <=2) {
			rain = !rain;
		}
		// temperatura:
		// scende di 1 grado al 10%
		// scende di 0.5 gradi al 10%
		// sale di 1 grado al 10%
		// sale di 0.5 gradi al 10%
		// rimane stabile al 60%
		next = r.nextInt(10)+1;
		switch (next) {
		case 1: temp -=1;
		break;
		case 2: temp -=0.5;
		break;
		case 3: temp +=0.5;
		break;
		case 4: temp +=1;
		break;
		}
		
		// ogni step e' un secondo
		seconds++;
	}

	public long getSecondsSinceStart() {
		return seconds;
	}

}
