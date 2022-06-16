function bpm = pulse_detector()

    bpm = [];
    data = readtable("hr.xlsx");
    y = data.Var2(1400:3900);
    
    hr_threshold = 700;
    samples_per_second = 100;
    seconds_per_minute = 60;
    
    previous_sample = 0;
    sample_counter = 0;
    peak_detected = 0;
    
    for i = 1:length(y)
       current_sample = y(i);
       if(current_sample - previous_sample) > hr_threshold && peak_detected == 0
           peak_detected = 1;
       elseif (current_sample - previous_sample) > hr_threshold && peak_detected == 1
           num_seconds_between_peaks = sample_counter / samples_per_second;
           bpm(end+1) = round(seconds_per_minute / num_seconds_between_peaks);
           sample_counter = 0;
       end
       if peak_detected == 1
           sample_counter = sample_counter + 1;
       end
       previous_sample = current_sample;
    end

end