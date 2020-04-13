function client(port)
%   provides a menu for accessing PIC32 motor control functions
%
%   client(port)
%
%   Input Arguments:
%       port - the name of the com port.  This should be the same as what
%               you use in screen or putty in quotes ' '
%
%   Example:
%       client('/dev/ttyUSB0') (Linux/Mac)
%       client('COM3') (PC)
%
%   For convenience, you may want to change this so that the port is hardcoded.
   
% Opening COM connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('Opening port %s....\n',port);

% settings for opening the serial port. baud rate 230400, hardware flow control
% wait up to 120 seconds for data before timing out
mySerial = serial(port, 'BaudRate', 230400, 'FlowControl', 'hardware','Timeout',10); 
% opens serial connection
fopen(mySerial);
% closes serial port when function exits
clean = onCleanup(@()fclose(mySerial));                                 

has_quit = false;
% menu loop
while ~has_quit
    fprintf('PIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf(['a: Read current sensor (ADCcounts)     b: Read current sensor (mA)\n'...
        'c: Read encoder (counts)               d: Read encoder (deg)\n'...
        'e: Reset encoder                       f: Set PWM (-100 to 100)\n'...
        'g: Set current gains                   h: Get current gains\n'...
        'i: Set position gains                  j: Get position gains\n'...
        'k: Test current control                l: Go to angle (deg)\n'...
        'm: Load step trajectory                n: Load cubic trajectory\n'...
        'o: Execute Trajectory                  p: Unpower the motor\n'...
        'q: Quit                                r: Get mode\n'...
        's: Test FIR Filter                     x: Add\n'...
        'z: Dummy Command\n']);
    
    % read the user's choice
    selection = input('\nENTER COMMAND: ', 's');
     
    % send the command to the PIC32
    fprintf(mySerial,'%c\n',selection);
    
    % take the appropriate action
    switch selection
        case 'a'
            n = fscanf(mySerial, '%d');
            fprintf('The motor current is %d ADC counts.\n',n);
        case 'b'
            n = fscanf(mySerial, '%d');
            fprintf('The motor current is %d mA\n',n);
        case 'c' %COMPLETE
            n = fscanf(mySerial, '%d');
            fprintf('The motor angle is %d counts.\n',n);
        case 'd'%COMPLETE
            n = fscanf(mySerial, '%d'); %consider changing to float instead of int
            fprintf('The motor angle is %d degrees.\n',n);
        case 'e' %COMPLETE
            %computation will be handled by .c. May change to return motor angle in degrees
        case 'f'
            n = input('What PWM percentage would you like [-100 100]? ');
            fprintf(mySerial,'%d\n',n);
            if n < 0 && n >= -100
                %!!! currently doesn't display properly
                fprintf('PWM has been set to %d%% in the clockwise direction.\n',n);
            elseif n >= 0 && n <= 100
                fprintf('PWM has been set to %d%% in the counterclockwise direction.\n',n);
            else
                fprintf('PWM percentage is out of range, try again.\n');
            end
        case 'g' %COMPLETE
            n = input('Enter your desired Kp current gain [recommended: 4.76]: ');
            m = input('Enter your desired Ki current gain [recommended: 0.32]: ');
            str = string({n,m});
            str = join(str);
            fprintf('Sending Kp = %f and Ki = %f to the current controller.\n',n,m);
            fprintf(mySerial,'%s\n',str);
        case 'h' %COMPLETE
            n = fscanf(mySerial,'%f');
            fprintf('The current controller is using Kp = %f and Ki = %f.\n', n(1), n(2));
        case 'i' %COMPLETE
            n = input('Enter your desired Kp current gain [recommended: 4.76]: ');
            m = input('Enter your desired Ki current gain [recommended: 0.32]: ');
            o = input('Enter your desired Kd current gain [recommended: 10.63]: ');
            str = string({n,m,o});
            str = join(str);
            fprintf('Sending Kp = %f, Ki = %f, and Kd = %f to the current controller.\n',n,m,o);
            fprintf(mySerial,'%s\n',str);
        case 'j' %COMPLETE
            n = fscanf(mySerial,'%f');
            fprintf('The current controller is using Kp = %f, Ki = %f, and Kd = %f.\n', n(1), n(2), n(3));
        case 'k'
            %get motor current data from PIC32 then plot it
        case 'l'
            n = input('Enter the desired motor angle in degrees: ');
            fprintf(mySerial,'%d',n);
            fprintf('Motor moving to %d degrees.\n',n);
        case 'm' %COMPLETE
            n = input('Enter step trajectory as an nx2 array, in seconds and degrees [time1, ang1; time2, ang2; ...]: ');
            t = 0;
            valid = true;
            for i = 1:size(n,1)
                if n(i,1) < t || n(i,1) > 10
                    valid = false;
                end
                t = n(i,1);
            end
            if valid
                fprintf('Plotting the desired trajectory and sending to the PIC32... ');
                ref = genRef(n,'step'); %!!genRef does not start from 0. Ex: [1,45; 2,90] will start at 45 @ t=0
                m = length(ref);
                fprintf(mySerial, '%d\n', m);
                for i = 1:m
                    fprintf(mySerial, '%f\n', ref(i));
                end
                fprintf('completed.\n')
%                 trajectory = zeros(1,m); %uncomment for debugging
%                 for i = 1:m
%                     trajectory(:,i) = fscanf(mySerial, '%f');
%                 end
            else
                fprintf('Error: Maximum trajectory time is 10 seconds.\n');
            end
        case 'n' %COMPLETE
            n = input('Enter step trajectory as an nx2 array, in seconds and degrees [time1, ang1; time2, ang2; ...]: ');
            t = 0;
            valid = true;
            for i = 1:size(n,1)
                if n(i,1) < t || n(i,1) > 10
                    valid = false;
                end
                t = n(i,1);
            end
            if valid
                fprintf('Plotting the desired trajectory and sending to the PIC32... ');
                ref = genRef(n,'cubic'); %!!genRef does not start from 0. Ex: [1,45; 2,90] will start at 45 @ t=0
                m = length(ref);
                fprintf(mySerial, '%d\n', m);
                for i = 1:m
                    fprintf(mySerial, '%f\n', ref(i));
                end
                fprintf('completed.\n')
%                 trajectory = zeros(1,m); %uncomment for debugging
%                 for i = 1:m
%                     trajectory(:,i) = fscanf(mySerial, '%f');
%                 end
            else
                fprintf('Error: Maximum trajectory time is 10 seconds.\n');
            end
        case 'o'
            %do nothing. PIC will do computations. should probably remove case statement
        case 'p' %COMPLETE
            %do nothing. PIC will do computations
        case 'q' %COMPLETE
            has_quit = true;
        case 'r'
        case 's' %COMPLETE
            p = fscanf(mySerial, '%d'); %!!!This is buggy - sometimes doesn't return NUM_SAMPLES
            signals = zeros(p,2);
            for i = 1:p
                signals(i,:) = fscanf(mySerial,'%f');
            end
            fprintf('Signals recieved\n');
            hold on
            plot(signals(:,1))
            plot(signals(:,2))
            hold off
        case 'x'                         % example operation
            p = input('Enter two numbers (separated by a space): ', 's'); % get the number to send
            fprintf(mySerial, '%s\n',p); % send the number
            p = fscanf(mySerial,'%d');   % get the incremented number back
            fprintf('Read: %d\n',p);     % print it to the screen
        case 'z'                         % example operation
            n = input('Enter number: '); % get the number to send
            fprintf(mySerial, '%d\n',n); % send the number
            n = fscanf(mySerial,'%d');   % get the incremented number back
            fprintf('Read: %d\n',n);     % print it to the screen
        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end
