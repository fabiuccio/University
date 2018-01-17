package mystubs;

import java.util.Observable;

import hcs.interfaces.ClockObserver;
import hcs.model.Clock;
import stubs.IPhysicalBoiler;

/**
 * Implements a physical boiler.
 */
public class PhysBoiler implements IPhysicalBoiler, ClockObserver {

	private double currentTemperature;
	private double targetTemperature;
	
	/** Creates a new <code>Boiler</code> with target and current temperature set to 0 */
	public PhysBoiler() {
		targetTemperature = 0;
		currentTemperature = 0;
		Clock.getInstance().addObserver(this);
	}
	
	/**
	 * Creates a new <code>Boiler</code> with the given initial temperature and target temperature set to 0.
	 * If a different target temperature must be set, the <code>setTargetTemperature()</code> can be used.
	 * @param initialTemperature the initial current temperature.
	 */
	public PhysBoiler(double initialTemperature) {
		targetTemperature = 0;
		currentTemperature = initialTemperature;
		Clock.getInstance().addObserver(this);
	}

	public double getCurrentTemperature() {
		return currentTemperature;
	}

	public void setTargetTemperature(double temperature) {
		targetTemperature = temperature;
	}

	public void update(Observable o, Object arg) {
		if (currentTemperature < targetTemperature) {
			currentTemperature++;
		} else if (currentTemperature > targetTemperature) {
			currentTemperature--;
		}
	}

	public double getTargetTemperature() {
		return targetTemperature;
	}
}
